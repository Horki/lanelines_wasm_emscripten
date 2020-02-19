#include <iostream>
#include <vector>
#include <cmath>

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

int main() {
  cv::Mat img_original = cv::imread("img.jpg");
  cv::Mat img_gray, img_gauss, img_canny;
  // 1. Convert to gray
  cv::cvtColor(img_original, img_gray, cv::COLOR_RGB2GRAY);

  // 2. Convert to gaussian
  cv::Size ksize = cv::Size(5, 5);
  double sigma_x = 0.0;
  double sigma_y = 0.0;
  cv::GaussianBlur(img_gray, img_gauss, ksize, sigma_x, sigma_y);

  // 3. Canny
  double min_thresh = 128.0;
  double max_thresh = 130.0;
  int aperture      = 3;
  cv::Canny(img_gauss, img_canny, min_thresh, max_thresh, aperture);
}
