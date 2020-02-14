// STD
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>

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
public:
    LaneLines(emscripten::val const & js_image) {
        convert_to_mat(js_image);
    }
    ~LaneLines() {
        std::cout << "Here I will do some memory GC" << std::endl;
    }
    Imaag to_gray() {
        cv::cvtColor(img_original, img_current, cv::COLOR_RGBA2GRAY);
        return to_imag();
    }
    Imaag to_gaussian(int kernel, double sigma_x, double sigma_y) {
        cv::GaussianBlur(img_current, img_current,
                cv::Size(kernel, kernel), sigma_x, sigma_y);
        return to_imag();
    }
    Imaag to_canny(double threshold_1, double threshold_2, int aperture) {
        cv::Canny(img_current, img_current, threshold_1, threshold_2, aperture);
        return to_imag();
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
    Imaag to_imag() {
        // https://emscripten.org/docs/porting/emscripten-runtime-environment.html#emscripten-memory-model
        int size = img_current.cols * img_current.rows * img_current.channels();
        return {
            img_current.cols,                       // Width
            img_current.rows,                       // Height
            img_current.channels(),                 // Channels
            size,                                   // Size
            reinterpret_cast<int>(img_current.data) // Pointer to image address
        };
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
      .function("to_canny",    &LaneLines::to_canny,    emscripten::allow_raw_pointers());
  emscripten::value_object<Imaag>("Imaag")
    .field("width",    &Imaag::width)
    .field("height",   &Imaag::height)
    .field("channels", &Imaag::channels)
    .field("size",     &Imaag::size)
    .field("p_addr",   &Imaag::p_addr);
};
