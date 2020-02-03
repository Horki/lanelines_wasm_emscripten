#include <iostream>
#include <string>

#include <emscripten/bind.h>

#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>

void test() {
  std::cout << CV_MAJOR_VERSION << std::endl;
}

EMSCRIPTEN_BINDINGS(my_module) {
  emscripten::function("test", &test);
}
