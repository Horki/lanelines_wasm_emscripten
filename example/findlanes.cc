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
  cv::Mat img_gray, img_gauss, img_canny, img_res;
  img_original.copyTo(img_res);
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

  // 4. Hough
  cv::Mat mask = cv::Mat::zeros(img_canny.rows, img_canny.cols, img_canny.channels());
  int x_size = img_canny.cols;
  int y_size = img_canny.rows;
  int x_a = int(x_size * 0.5);
  int x_b = int(x_size * 0.05);
  int y_a = int(y_size * 0.62);

  //https://docs.opencv.org/3.4/d3/d96/tutorial_basic_geometric_drawing.html
  cv::Point ppt[1][4];
  ppt[0][0] = cv::Point(0, y_size);
  ppt[0][1] = cv::Point((x_a - x_b), y_a);
  ppt[0][2] = cv::Point((x_a + x_b), y_a);
  ppt[0][3] = cv::Point(x_size, y_size);
  const cv::Point * p_ppt[1] = { ppt[0] };
  int npt[] = {20};
  cv::fillPoly(mask, p_ppt, npt, 1, cv::Scalar(255, 255, 255));
  cv::Mat masked_edges;
  cv::bitwise_and(img_canny, img_canny, masked_edges, mask);

  double rho = 1.0;
  double theta = CV_PI / 180.0;
  int threshold = 55;
  double min_theta =  50.0;
  double max_theta = 150.0;
  std::vector<cv::Vec4i> lines;
  cv::HoughLinesP(masked_edges, lines, rho, theta, threshold, min_theta, max_theta);

  for (const cv::Vec4i & l : lines) {
    cv::line(img_res,
             cv::Point(l[0], l[1]),
             cv::Point(l[2], l[3]),
             cv::Scalar(0, 0, 255), // red
             5                      // thickness
    );
  }

  // load an image
  cv::imshow("original",     img_original);
  cv::imshow("gray",         img_gray);
  cv::imshow("gauss",        img_gauss);
  cv::imshow("canny",        img_canny);
  cv::imshow("masked edges", masked_edges);
  cv::imshow("result",       img_res);

  cv::waitKey(0);
}
