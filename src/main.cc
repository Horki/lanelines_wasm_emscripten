#include <algorithm>
#include <iostream>
#include <iterator>

#include "lanelines.hh"

// OpenCV
#include <opencv2/core/version.hpp>

// Emscripten
#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/emscripten.h>
#include <emscripten/val.h>

namespace testing_ex {
void test() { std::cout << CV_MAJOR_VERSION << std::endl; }

void print_file(const int &addr, const std::size_t &len) {
  char *data = reinterpret_cast<char *>(addr);
  std::copy(data, data + len, std::ostream_iterator<char>(std::cout, ""));
  std::cout << std::endl;
}

int increment(int n) { return ++n; }
}  // namespace testing_ex

EMSCRIPTEN_BINDINGS(my_module) {
  using namespace testing_ex;
  emscripten::function("test", &test);
  emscripten::function("incr", &increment);
  emscripten::function("print_file", &print_file);

  emscripten::class_<LaneLines>("LaneLines")
      .constructor<emscripten::val const &>()
      .function("toGray", &LaneLines::toGray)
      .function("toGaussian", &LaneLines::toGaussian)
      .function("toCanny", &LaneLines::toCanny)
      .function("toRegion", &LaneLines::toRegion)
      .function("toHoughes", &LaneLines::toHoughes)
      .function("toNext", &LaneLines::toNext)
      .function("getImaag", &LaneLines::getImaag);

  emscripten::value_object<Imaag>("Imaag")
      .field("width", &Imaag::width)
      .field("height", &Imaag::height)
      .field("channels", &Imaag::channels)
      .field("size", &Imaag::size)
      .field("p_addr", &Imaag::p_addr);
};
