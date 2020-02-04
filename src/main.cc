// STD
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>

// Emscripten
#include <emscripten/bind.h>

// OpenCV
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>

void test() {
  std::cout << CV_MAJOR_VERSION << std::endl;
}

void print_file(const int & addr, const std::size_t & len) {
    char * data = reinterpret_cast<char *>(addr);
    std::copy(data, data + len, std::ostream_iterator<char>(std::cout, ""));
    std::cout << std::endl;
}

int increment(int n) { return ++n; }

EMSCRIPTEN_BINDINGS(my_module) {
  emscripten::function("test", &test);
  emscripten::function("incr", &increment);
  emscripten::function("print_file", &print_file);
}
