// STD
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string_view>

// Emscripten
#include <emscripten/val.h>

// OpenCV
#include <opencv2/core.hpp>

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
  LaneLines(emscripten::val const& js_image);
  LaneLines(const cv::Mat& img);
  LaneLines(cv::Mat&& img);
  ~LaneLines();
  void toGray();
  void toGaussian(int kernel, double sigma_x, double sigma_y);
  void toCanny(double threshold_1, double threshold_2, int aperture);
  void toRegion(size_t x_1, size_t y_1, size_t x_2, size_t y_2);
  void toHoughes(double rho, int threshold, double min_theta, double max_theta,
                 int thickness);
  void toNext();
  Imaag getImaag() const;

 private:
  // TODO: Find a better way!
  void convertToMat(emscripten::val const& js_image);
};
