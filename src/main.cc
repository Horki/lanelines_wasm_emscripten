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
    int p_addr;
};

void test() {
  std::cout << CV_MAJOR_VERSION << std::endl;
}

cv::Mat convert_to_mat(emscripten::val const & js_image) {
    auto w       = js_image["width"].as<unsigned long>();
    auto h       = js_image["height"].as<unsigned long>();
    auto imgData = js_image["data"].as<emscripten::val>();
    cv::Mat img(h, w, CV_8UC4);
    auto length  = imgData["length"].as<unsigned int>();
    std::uint8_t * destBuffer = img.data;
    emscripten::val memoryView(emscripten::typed_memory_view(length, destBuffer));
    memoryView.call<void>("set", imgData);
    return img;
}

Imaag testing(emscripten::val const & js_image) {
    cv::Mat img = convert_to_mat(js_image);
    cv::Mat img_gray;
    cv::cvtColor(img, img_gray, cv::COLOR_RGBA2GRAY);

    return {
        img_gray.cols,
        img_gray.rows,
        img_gray.channels(),
        (img_gray.cols * img_gray.rows * img_gray.channels()),
        reinterpret_cast<int>(img_gray.data)
    };
}

void print_file(const int & addr, const std::size_t & len) {
    char * data = reinterpret_cast<char *>(addr);
    std::copy(data, data + len, std::ostream_iterator<char>(std::cout, ""));
    std::cout << std::endl;
}

int increment(int n) { return ++n; }

EMSCRIPTEN_BINDINGS(my_module) {
  emscripten::function("test",          &test);
  emscripten::function("incr",          &increment);
  emscripten::function("print_file",    &print_file);
  emscripten::function("testing",       &testing, emscripten::allow_raw_pointers());

  emscripten::value_object<Imaag>("Imaag")
    .field("width",    &Imaag::width)
    .field("height",   &Imaag::height)
    .field("channels", &Imaag::channels)
    .field("size",     &Imaag::size)
    .field("p_addr",   &Imaag::p_addr);
}
