// STD
#include <cassert>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string_view>
#include <vector>

// Emscripten
#include <emscripten/val.h>

// OpenCV
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

struct Imaag {
  int width;
  int height;
  int channels;
  int size;
  int p_addr;  // Stored in HEAP
};

class TimeDiff {
 private:
  std::clock_t start, stop;
  std::chrono::high_resolution_clock::time_point t_start, t_stop;
  std::string_view s;

 public:
  TimeDiff(std::string_view s)
      : s(s),
        start(std::clock()),
        t_start(std::chrono::high_resolution_clock::now()) {}
  ~TimeDiff() {
    stop = std::clock();
    t_stop = std::chrono::high_resolution_clock::now();
    auto t_duration =
        std::chrono::duration<double, std::milli>(t_stop - t_start).count();
    auto duration = stop - start;
    std::cout << std::fixed << std::setprecision(2) << s
              << "it took: " << duration << " clicks, "
              << " CPU time used: " << ((1000.0 * duration) / CLOCKS_PER_SEC)
              << " ms; Wall clock time " << t_duration << " ms" << std::endl;
  }
};

class LaneLines {
 private:
  cv::Mat img_original;
  cv::Mat img_current;
  cv::Mat img_buffer;

 public:
  LaneLines(const emscripten::val&);
  LaneLines(const cv::Mat&);
  LaneLines(cv::Mat&&);
  ~LaneLines();
  void toGray();
  void toGaussian(const int, const double, const double);
  void toCanny(const double, const double, const int);
  void toRegion(const std::size_t, const std::size_t, const std::size_t,
                const std::size_t);
  void toHoughes(const double, const int, const double, const double,
                 const int);
  void toNext();
  Imaag getImaag() const;

 private:
  // TODO: Find a better way!
  void convertToMat(const emscripten::val&);
};
