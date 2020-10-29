#include <opencv2/opencv.hpp>

// Show the image zoomed, for it was way too large for display
void showZoomed(const cv::Mat& src, const char* name) {
  using namespace cv;
  Mat dst;
  resize(src, dst, Size(), 0.3, 0.3, CV_INTER_AREA);
  namedWindow(name);
  imshow(name, dst);
}

float getAngle(const cv::RotatedRect& r) {
  return r.size.width < r.size.height ? r.angle : r.angle + 90;
}

float getLengthWidthRation(const cv::RotatedRect& r) {
  float ratio = r.size.width / r.size.height;
  return ratio >= 1 ? ratio : 1 / ratio;
}

float getDistance(const cv::Point2f& l, const cv::Point2f& r) {
  return std::hypot(l.x - r.x, l.y - r.y);
}

float getLength(const cv::RotatedRect& rect) {
  return std::max(rect.size.height, rect.size.width);
}

void drawRotatedRect(cv::Mat& output, const cv::RotatedRect& rect,
                     const cv::Scalar& color) {
  std::array<cv::Point2f, 4> vertices;
  rect.points(vertices.data());
  for (int i = 0; i < 4; i++) {
    cv::line(output, vertices[i], vertices[(i + 1) % 4], color, 10);
  }
}

cv::Scalar getRandomColor(cv::RNG& rng) {
  return {rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256)};
}

int main() {
  using namespace cv;
  using namespace std;
  RNG rng(time(NULL));
  Mat src = imread("../task 2/grey.png");
  Mat colored = imread("../task 2/test.jpg");

  // Find the contours
  Mat grey, blurred, thresholded;
  cvtColor(src, grey, CV_BGR2GRAY);
  blur(grey, blurred, Size(5, 5));
  threshold(blurred, thresholded, 150, 250, THRESH_TOZERO);

  // showZoomed(src, "src");
  // showZoomed(blurred, "blurred");
  showZoomed(thresholded, "thresh");

  vector<vector<Point>> contours;
  findContours(thresholded, contours, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
  vector<RotatedRect> rects;
  for (int i = 0; i < contours.size(); ++i) {
    auto rect = minAreaRect(contours[i]);
    if (rect.size.area() < 1000) continue;
    auto angle = getAngle(rect);
    if (angle < -30 || angle > 30) continue;
    if (getLengthWidthRation(rect) < 2) continue;
    
    // Filtered
    Scalar color = getRandomColor(rng);
    drawContours(src, contours, i, color, 3);
    // drawRotatedRect(src, rect, color);
    rects.push_back(move(rect));
  }

  sort(rects.begin(), rects.end(),
       [](const RotatedRect& l, const RotatedRect& r) {
         return getAngle(l) < getAngle(r);
       });

  showZoomed(src, "contours");

  // Pair
  struct Board {
    RotatedRect l;
    RotatedRect r;
    RotatedRect b;
  };
  vector<Board> boards;
  // Note the size of lights is changing because elements are removed
  for (int i = 0; i < rects.size(); i++) {
    vector<int> candidates;
    for (int j = i + 1; j < rects.size(); j++) {
      if (abs(getAngle(rects[i]) - getAngle(rects[j])) > 10) continue;
      auto length_ratio = getLength(rects[i]) / getLength(rects[j]);
      if (length_ratio < 1) length_ratio = 1 / length_ratio;
      if (length_ratio > 2) continue;

      candidates.push_back(j);
    }

    auto center = rects[i].center;
    auto chosen = min_element(candidates.cbegin(), candidates.cend(),
                              [center, &rects](int l, int r) {
                                return getDistance(center, rects[l].center) <
                                       getDistance(center, rects[r].center);
                              });
    if (chosen != candidates.cend()) {
      RotatedRect board{
          (rects[i].center + rects[*chosen].center) / 2,
          Size(getDistance(rects[i].center, rects[*chosen].center),
               1.5 * (getLength(rects[i]) + getLength(rects[*chosen]) / 2)),
          (getAngle(rects[i]) + getAngle(rects[*chosen])) / 2};
      boards.push_back({rects[i], rects[*chosen], board});
      auto color = getRandomColor(rng);
      drawRotatedRect(colored, boards.back().l, color);
      drawRotatedRect(colored, boards.back().r, color);
      drawRotatedRect(colored, boards.back().b, color);
      cv::line(colored, boards.back().l.center, boards.back().r.center, color, 3);

      rects.erase(rects.cbegin() + *chosen);
    }
  }
  showZoomed(colored, "result");

  waitKey();
  return 0;
}
