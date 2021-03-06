#include "lanelines.hh"

LaneLines::LaneLines(emscripten::val const& js_image, const State& s) {
  state = s;
  auto t = std::make_unique<TimeDiff>("cc: Constructor: ");
  convertToMat(js_image);
  img_original.copyTo(img_current);  // copy
}

void LaneLines::toGray() {
  // TODO: figure out more elegant way for state validation
  assert(state == State::INIT);
  auto t = std::make_unique<TimeDiff>("cc: toGray(): ");
  cv::cvtColor(img_current, img_buffer, cv::COLOR_RGBA2GRAY);
}

void LaneLines::toGaussian(const int kernel, const double sigma_x,
                           const double sigma_y) {
  assert(state == State::GRAY);
  auto t = std::make_unique<TimeDiff>("cc: toGaussian() ");
  img_buffer.release();
  cv::GaussianBlur(img_current, img_buffer, cv::Size(kernel, kernel), sigma_x,
                   sigma_y);
}

void LaneLines::toCanny(const double threshold_1, const double threshold_2,
                        const int aperture) {
  assert(state == State::GAUSSIAN);
  auto t = std::make_unique<TimeDiff>("cc: toCanny() ");
  img_buffer.release();
  cv::Canny(img_current, img_buffer, threshold_1, threshold_2, aperture);
}

void LaneLines::toRegion(const int x_1, const int y_1, const int x_2,
                         const int y_2) {
  assert(state == State::CANNY);
  auto t = std::make_unique<TimeDiff>("cc: toRegion() ");
  img_buffer.release();
  // https://docs.opencv.org/3.4/d3/d96/tutorial_basic_geometric_drawing.html
  // TODO: Remove asserts in future!
  assert(x_1 <= img_current.cols);
  assert(y_1 <= img_current.rows);
  assert(x_2 <= img_current.cols);
  assert(y_2 <= img_current.rows);
  const cv::Mat mask{cv::Mat::zeros(img_current.rows, img_current.cols,
                                    img_current.channels())};
  std::cout << "cc: x_1: " << x_1 << ", y_1: " << y_1 << std::endl;
  std::cout << "cc: x_2: " << x_2 << ", y_2: " << y_2 << std::endl;
  const std::array<cv::Point, 4> points{
      cv::Point(0, img_current.rows), cv::Point(x_1, y_1), cv::Point(x_2, y_2),
      cv::Point(img_current.cols, img_current.rows)};
  cv::fillPoly(mask, points, cv::Scalar(255, 255, 255));
  // select from canny image by polygon
  cv::bitwise_and(img_current, img_current, img_buffer, mask);
}

void LaneLines::toHoughes(const double rho, const int threshold,
                          const double min_theta, const double max_theta,
                          const int thickness) {
  assert(state==State::REGION);
  auto t = std::make_unique<TimeDiff>("cc: toHoughes() ");
  std::vector<cv::Vec4i> lines;
  cv::HoughLinesP(img_current, lines, rho, theta(), threshold, min_theta,
                  max_theta);
  img_original.copyTo(img_buffer);

  const auto red_color{cv::Scalar{255, 0, 0, 255}};

  for (const auto& l : lines) {
    cv::line(img_buffer, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]),
             red_color, thickness);
  }
}

void LaneLines::toNext() {
  auto t = std::make_unique<TimeDiff>("cc: toNext() ");
  img_buffer.copyTo(img_current);
  ++state;
}

Imaag LaneLines::getImaag() const {
  auto t = std::make_unique<TimeDiff>("cc: getImaag() ");
  // https://emscripten.org/docs/porting/emscripten-runtime-environment.html#emscripten-memory-model
  auto size{img_buffer.cols * img_buffer.rows * img_buffer.channels()};
  return {
      img_buffer.cols,                        // Width
      img_buffer.rows,                        // Height
      img_buffer.channels(),                  // Channels
      size,                                   // Size
      reinterpret_cast<int>(img_buffer.data)  // Pointer to image address
  };
}

// private
// TODO: Find a better way!
void LaneLines::convertToMat(const emscripten::val& js_image) {
  auto t = std::make_unique<TimeDiff>("cc: convertToMat() ");
  auto w{js_image["width"].as<int>()};
  auto h{js_image["height"].as<int>()};
  auto imgData{js_image["data"].as<emscripten::val>()};
  img_original.create(h, w, CV_8UC4);
  auto length{imgData["length"].as<std::size_t>()};
  emscripten::val memoryView{
      emscripten::typed_memory_view(length, img_original.data)};
  memoryView.call<void>("set", imgData);
}
