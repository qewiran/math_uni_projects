#include <line_algorithm.hpp>
#include <opencv2/core/mat.hpp>
std::vector<cv::Point2i> bresenhamLineUp(cv::Mat img, cv::Point2i p1,
                                         cv::Point2i p2, size_t color = 255) {
  std::vector<cv::Point2i> result{};
  int dx = p2.x - p1.x;
  int dy = p2.y - p1.y;

  int x = p1.x;

  int x_step = 1;
  int y_step = 1;

  if (dx < 0) {
    x_step *= -1;
    dx *= -1;
  }

  int D = 2 * dx - dy;

  for (int y = p1.y; y < p2.y; y += y_step) {
    result.emplace_back(y, x);
    img.at<u_char>(y, x) = color;
    // result.emplace_back(x, y);
    // img.at<u_char>(x, y) = color;
    if (D > 0) {
      x += x_step;
      D += 2 * (dx - dy);
    } else
      D += 2 * dx;
  }
  return result;
}

std::vector<cv::Point2i> bresenhamLineDown(cv::Mat img, cv::Point2i p1,
                                           cv::Point2i p2, size_t color = 255) {
  std::vector<cv::Point2i> result{};
  int dx = p2.x - p1.x;
  int dy = p2.y - p1.y;

  int y = p1.y;

  int x_step = 1;
  int y_step = 1;

  if (dy < 0) {
    y_step *= -1;
    dy *= -1;
  }

  int D = 2 * dy - dx;

  for (int x = p1.x; x < p2.x; x += x_step) {
    result.emplace_back(y, x);
    img.at<u_char>(y, x) = color;
    // result.emplace_back(x, y);
    // img.at<u_char>(x, y) = color;
    if (D > 0) {
      y += y_step;
      D += 2 * (dy - dx);
    } else
      D += 2 * dy;
  }
  return result;
}

std::vector<cv::Point2i> bresenhamLine(cv::Mat img, cv::Point2i p1,
                                       cv::Point2i p2, size_t color = 255) {
  int y2 = p2.y;
  int y1 = p1.y;
  int x2 = p2.x;
  int x1 = p1.x;

  if (std::abs(y2 - y1) < std::abs(x2 - x1)) {
    if (x1 > x2)
      return bresenhamLineDown(img, p2, p1, color);
    else
      return bresenhamLineDown(img, p1, p2, color);
  } else {
    if (y1 > y2)
      return bresenhamLineUp(img, p2, p1, color);
    else
      return bresenhamLineUp(img, p1, p2, color);
  }
}

void dda(cv::Mat img, cv::Point2i p1, cv::Point2i p2) {
  int dx = p2.x - p1.x;
  int dy = p2.y - p1.y;

  double y = p1.y;
  double x = p1.x;

  int steps = std::abs(dx) > std::abs(dy) ? std::abs(dx) : std::abs(dy);

  double x_increment = static_cast<double>(dx) / steps;
  double y_increment = static_cast<double>(dy) / steps;

  for (size_t i = 0; i < steps; ++i) {

    img.at<uchar>(std::round(y), std::round(x)) = 255;
    x += x_increment;
    y += y_increment;
  }
}

void symmetryDots(cv::Mat img, cv::Point2i center, cv::Point2i p) {
  img.at<uchar>(center.y + p.y, center.x + p.x) = 255;
  img.at<uchar>(center.y + p.y, center.x - p.x) = 255;
  img.at<uchar>(center.y - p.y, center.x + p.x) = 255;
  img.at<uchar>(center.y - p.y, center.x - p.x) = 255;
  img.at<uchar>(center.y + p.x, center.x + p.y) = 255;
  img.at<uchar>(center.y + p.x, center.x - p.y) = 255;
  img.at<uchar>(center.y - p.x, center.x + p.y) = 255;
  img.at<uchar>(center.y - p.x, center.x - p.y) = 255;
}

void drawCircle(cv::Mat img, cv::Point2i center, int radius) {
  int x = 0;
  int y = radius;
  int d = 3 - 2 * radius;
  symmetryDots(img, center, cv::Point2i(x, y));

  while (y >= x) {
    ++x;
    if (d > 0) {
      --y;
      d += 5 * (x - y) + 10;
    } else
      d += 4 * x + 6;
    symmetryDots(img, center, cv::Point2i(x, y));
  }
}
