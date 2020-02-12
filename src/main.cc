// STD
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>
//#include <vector>

// Emscripten
#include <emscripten.h>
//#include <emscripten/bind.h>

// OpenCV
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <stdio.h>
#include <cstdlib>
#include <cstring> // memcpy

typedef std::vector<double> Corner;
typedef std::vector<Corner> Rectangle;

void test() {
  std::cout << CV_MAJOR_VERSION << std::endl;
}

//struct Imaag {
//    int width;
//    int height;
//    int channels;
//    int addr;
//};

//Imaag convert_image(const int & addr, int w, int h) {
// TODO: Temporary, fix to C++ solution later!!!
#ifdef __cplusplus
extern "C" {
#endif
EMSCRIPTEN_KEEPALIVE
int * convert_image(uchar * data, int w, int h) {
    std::cout << "w = " << w << ", h = " << h << std::endl;
//    uchar * data = reinterpret_cast<uchar *>(addr);
//    printf("w = %d, h = %d\n", w, h);
    cv::Mat image(h, w, CV_8UC4, data);
    cv::Mat imgGray;
//    cv::Mat matBinary;
    cv::cvtColor(image, imgGray, cv::COLOR_RGBA2GRAY);
    int return_image_1_width = imgGray.cols;
    int return_image_1_height = imgGray.rows;
    int return_image_1_channels = imgGray.channels();
    int return_image_1_size = return_image_1_width * return_image_1_height * return_image_1_channels;
    // Debug purpose!
    std::cout << " width " << return_image_1_width
              << " height " << return_image_1_height
              << " channels: " << return_image_1_channels
              << " size: " << return_image_1_size
              << std::endl;

    int return_data_size = 20 * sizeof(int) + return_image_1_size;
    int *return_data = reinterpret_cast<int*>(malloc(return_data_size));
    uchar *return_image_1_addr = reinterpret_cast<uchar*>(&return_data[20]);
    memcpy(return_image_1_addr, imgGray.data, return_image_1_size);

    return_data[0] = return_image_1_width;
    return_data[1] = return_image_1_height;
    return_data[2] = return_image_1_channels;
    return_data[3] = reinterpret_cast<int>(return_image_1_addr);
    return return_data;
//    return { return_image_1_width,
//             return_image_1_height,
//             return_image_1_channels,
//             reinterpret_cast<int>(return_image_1_addr)};
}
#ifdef __cplusplus
}
#endif

void print_file(const int & addr, const std::size_t & len) {
    char * data = reinterpret_cast<char *>(addr);
    std::copy(data, data + len, std::ostream_iterator<char>(std::cout, ""));
    std::cout << std::endl;
}

int increment(int n) { return ++n; }

//EMSCRIPTEN_BINDINGS(my_module) {
//  emscripten::function("test",          &test);
//  emscripten::function("incr",          &increment);
//  emscripten::function("print_file",    &print_file);
//  emscripten::function("convert_image", &convert_image);
//////  emscripten::function("convert_image", &convert_image, allow_raw_pointers());
//}
