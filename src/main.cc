// STD
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>
#include <vector>
#include <cassert>

// Emscripten
#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/emscripten.h>
#include <emscripten/val.h>

// OpenCV
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

struct Imaag {
    int width;
    int height;
    int channels;
    int size;
    int p_addr; // Stored in HEAP
};

class LaneLines {
private:
    cv::Mat img_original;
    cv::Mat img_current;
    cv::Mat img_buffer;
public:
    LaneLines(emscripten::val const & js_image) {
        convert_to_mat(js_image);
        img_original.copyTo(img_current); // copy
    }
    LaneLines(const cv::Mat & img) {
        img_original = img;
        img_buffer   = img;
    }
    LaneLines(cv::Mat && img) {
        img_original = std::move(img);
        img_original.copyTo(img_buffer);
    }
    ~LaneLines() {
        std::cout << "Here I will do some memory GC" << std::endl;
    }
    void to_gray() {
        cv::cvtColor(img_current, img_buffer, cv::COLOR_RGBA2GRAY);
    }
    void to_gaussian(int kernel, double sigma_x, double sigma_y) {
        img_buffer.release();
        cv::GaussianBlur(img_current, img_buffer,
                cv::Size(kernel, kernel), sigma_x, sigma_y);
    }
    void to_canny(double threshold_1, double threshold_2, int aperture) {
        img_buffer.release();
        cv::Canny(img_current, img_buffer,
                threshold_1, threshold_2, aperture);
    }
    void to_region(size_t x_1, size_t y_1, size_t x_2, size_t y_2) {
        img_buffer.release();
        // https://docs.opencv.org/3.4/d3/d96/tutorial_basic_geometric_drawing.html
        assert(x_1 <= img_current.cols);
        assert(y_1 <= img_current.rows);
        assert(x_2 <= img_current.cols);
        assert(y_2 <= img_current.rows);
        cv::Mat mask = cv::Mat::zeros(img_current.rows, img_current.cols, img_current.channels());
        std::vector<std::vector<cv::Point>> points {{
            cv::Point(0           , img_current.rows),
            cv::Point(int(x_1)        , int(y_1)),
            cv::Point(int(x_2)        , int(y_2)),
            cv::Point(img_current.cols, img_current.rows)
        }};
        cv::fillPoly(mask, points, cv::Scalar(255, 255, 255));
        // select from canny image by polygon
        cv::bitwise_and(img_current, img_current, img_buffer, mask);
    }
    void to_houghes() {
        // TODO: add later
    }
    void to_next() {
        img_buffer.copyTo(img_current);
    }
    Imaag to_imaag() {
        // https://emscripten.org/docs/porting/emscripten-runtime-environment.html#emscripten-memory-model
        int size = img_buffer.cols * img_buffer.rows * img_buffer.channels();
        return {
                img_buffer.cols,                       // Width
                img_buffer.rows,                       // Height
                img_buffer.channels(),                 // Channels
                size,                                  // Size
                reinterpret_cast<int>(img_buffer.data) // Pointer to image address
        };
    }

private:
    // TODO: Find a better way!
    void convert_to_mat(emscripten::val const & js_image) {
        auto w       = js_image["width"].as<unsigned long>();
        auto h       = js_image["height"].as<unsigned long>();
        auto imgData = js_image["data"].as<emscripten::val>();
        img_original.create(h, w, CV_8UC4);
        auto length  = imgData["length"].as<unsigned int>();
        std::uint8_t * destBuffer = img_original.data;
        emscripten::val memoryView(emscripten::typed_memory_view(length, destBuffer));
        memoryView.call<void>("set", imgData);
    }
};

namespace testing_ex {
    void test() {
        std::cout << CV_MAJOR_VERSION << std::endl;
    }

    void print_file(const int &addr, const std::size_t &len) {
        char *data = reinterpret_cast<char *>(addr);
        std::copy(data, data + len, std::ostream_iterator<char>(std::cout, ""));
        std::cout << std::endl;
    }

    int increment(int n) { return ++n; }
}

EMSCRIPTEN_BINDINGS(my_module) {
  using namespace testing_ex;
  emscripten::function("test",          &test);
  emscripten::function("incr",          &increment);
  emscripten::function("print_file",    &print_file);

  emscripten::class_<LaneLines>("LaneLines")
      .constructor<emscripten::val const &>()
      .function("to_gray",     &LaneLines::to_gray,     emscripten::allow_raw_pointers())
      .function("to_gaussian", &LaneLines::to_gaussian, emscripten::allow_raw_pointers())
      .function("to_canny",    &LaneLines::to_canny,    emscripten::allow_raw_pointers())
      .function("to_region",   &LaneLines::to_region,   emscripten::allow_raw_pointers())
      .function("to_houghes",  &LaneLines::to_houghes,  emscripten::allow_raw_pointers())
      .function("to_next",     &LaneLines::to_next,     emscripten::allow_raw_pointers())
      .function("to_imaag",    &LaneLines::to_imaag,    emscripten::allow_raw_pointers());
  emscripten::value_object<Imaag>("Imaag")
    .field("width",    &Imaag::width)
    .field("height",   &Imaag::height)
    .field("channels", &Imaag::channels)
    .field("size",     &Imaag::size)
    .field("p_addr",   &Imaag::p_addr);
};
