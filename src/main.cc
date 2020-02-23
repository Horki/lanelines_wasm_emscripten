// STD
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>
#include <vector>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <cassert>

// Emscripten
#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/emscripten.h>
#include <emscripten/val.h>

// OpenCV
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>

struct Imaag {
    int width;
    int height;
    int channels;
    int size;
    int p_addr; // Stored in HEAP
};

class TimeDiff {
private:
    std::clock_t start, stop;
    std::chrono::high_resolution_clock::time_point t_start, t_stop;
    std::string s;
public:
    TimeDiff(const std::string & s) : s(s), start(std::clock()),
                 t_start(std::chrono::high_resolution_clock::now()) {}
    ~TimeDiff() {
        stop   = std::clock();
        t_stop = std::chrono::high_resolution_clock::now();
        auto t_duration = std::chrono::duration<double, std::milli>
                (t_stop - t_start).count();
        auto duration = stop - start;
        std::cout << std::fixed << std::setprecision(2)
                  << s
                  << "it took: " << duration << " clicks, "
                  << " CPU time used: "
                  << ((1000.0 * duration) / CLOCKS_PER_SEC)
                  << " ms; Wall clock time "
                  << t_duration
                  << " ms" << std::endl;
    }
};

class LaneLines {
private:
    cv::Mat img_original;
    cv::Mat img_current;
    cv::Mat img_buffer;
public:
    LaneLines(emscripten::val const & js_image) {
        TimeDiff t("cc: Constructor: ");
        convertToMat(js_image);
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
    void toGray() {
        TimeDiff t("cc: toGray(): ");
        cv::cvtColor(img_current, img_buffer, cv::COLOR_RGBA2GRAY);
    }
    void toGaussian(int kernel, double sigma_x, double sigma_y) {
        TimeDiff t("cc: toGaussian() ");
        img_buffer.release();
        cv::GaussianBlur(img_current, img_buffer,
                cv::Size(kernel, kernel), sigma_x, sigma_y);
    }
    void toCanny(double threshold_1, double threshold_2, int aperture) {
        TimeDiff t("cc: toCanny() ");
        img_buffer.release();
        cv::Canny(img_current, img_buffer,
                threshold_1, threshold_2, aperture);
    }
    void toRegion(size_t x_1, size_t y_1, size_t x_2, size_t y_2) {
        TimeDiff t("cc: toRegion() ");
        img_buffer.release();
        // https://docs.opencv.org/3.4/d3/d96/tutorial_basic_geometric_drawing.html
        assert(x_1 <= img_current.cols);
        assert(y_1 <= img_current.rows);
        assert(x_2 <= img_current.cols);
        assert(y_2 <= img_current.rows);
        cv::Mat mask = cv::Mat::zeros(img_current.rows, img_current.cols, img_current.channels());
        std::cout << "cc: x_1: " << x_1 << ", y_1: " << y_1 << std::endl;
        std::cout << "cc: x_2: " << x_2 << ", y_2: " << y_2 << std::endl;
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
    void toHoughes(double rho, int threshold,
                   double min_theta, double max_theta, int thickness) {
        TimeDiff t("cc: toHoughes() ");
        double theta = CV_PI / 180.0;
        std::vector<cv::Vec4i> lines;
        cv::HoughLinesP(img_current, lines, rho, theta, threshold, min_theta, max_theta);
        img_original.copyTo(img_buffer);

        for (const auto & l : lines) {
            cv::line(img_buffer,
                     cv::Point(l[0], l[1]),
                     cv::Point(l[2], l[3]),
                     cv::Scalar(0, 0, 255), // red
                     thickness
            );
        }
    }
    void toNext() {
        TimeDiff t("cc: toNext() ");
        img_buffer.copyTo(img_current);
    }
    Imaag getImaag() {
        TimeDiff t("cc: getImaag() ");
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
    void convertToMat(emscripten::val const & js_image) {
        TimeDiff t("cc: convertToMat() ");
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
      .function("toGray",     &LaneLines::toGray,     emscripten::allow_raw_pointers())
      .function("toGaussian", &LaneLines::toGaussian, emscripten::allow_raw_pointers())
      .function("toCanny",    &LaneLines::toCanny,    emscripten::allow_raw_pointers())
      .function("toRegion",   &LaneLines::toRegion,   emscripten::allow_raw_pointers())
      .function("toHoughes",  &LaneLines::toHoughes,  emscripten::allow_raw_pointers())
      .function("toNext",     &LaneLines::toNext,     emscripten::allow_raw_pointers())
      .function("getImaag",   &LaneLines::getImaag,   emscripten::allow_raw_pointers());
  emscripten::value_object<Imaag>("Imaag")
    .field("width",    &Imaag::width)
    .field("height",   &Imaag::height)
    .field("channels", &Imaag::channels)
    .field("size",     &Imaag::size)
    .field("p_addr",   &Imaag::p_addr);
};
