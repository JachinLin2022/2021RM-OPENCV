#include <cstdlib>
#include <opencv2/opencv.hpp>

const static int sample_size = 64;
#include "classifier.cpp"

void drawRotatedRect(cv::Mat& output, const cv::RotatedRect& rect,
                     const cv::Scalar& color) {
  std::array<cv::Point2f, 4> vertices;
  rect.points(vertices.data());
  for (int i = 0; i < 4; i++) {
    cv::line(output, vertices[i], vertices[(i + 1) % 4], color, 4);
  }
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

cv::Point2f getDirection(const cv::RotatedRect& rect) {
  auto angle = getAngle(rect) * 3.14159265358979323846f / 180.0f;
  return getLength(rect) * cv::Point2f(-sin(angle), fabs(cos(angle)));
}

void drawBox(cv::Mat& output, const std::vector<cv::Point2f>& box,
             const cv::Scalar& color) {
  for (int i = 0; i < 4; i++) {
    cv::line(output, box[i], box[(i + 1) % 4], color, 2);
  }
}

cv::Scalar getRandomColor(cv::RNG& rng) {
  return {static_cast<double>(rng.uniform(0, 256)),
          static_cast<double>(rng.uniform(0, 256)),
          static_cast<double>(rng.uniform(0, 256))};
}

std::vector<cv::Point2f> getPlate(const cv::RotatedRect& l,
                                  const cv::RotatedRect& r) {
  // 分辨左右
  if (l.center.x > r.center.x) {
    return getPlate(r, l);
  }
  auto left_dir = getDirection(l);
  auto right_dir = getDirection(r);
  return {l.center + left_dir, l.center - left_dir, r.center - right_dir,
          r.center + right_dir};
}

void normalizePlate(const cv::Mat& src, cv::Mat& dst,
                    const std::vector<cv::Point2f>& area) {
  using namespace cv;
  const std::vector<Point2f>& dst_area{Point2f(0, sample_size), Point2f(0, 0),
                                       Point2f(sample_size, 0),
                                       Point2f(sample_size, sample_size)};
  auto m = cv::getPerspectiveTransform(area, dst_area);
  warpPerspective(src, dst, m, Size(sample_size, sample_size));
}

cv::Mat object_dst;  // 全局变量，用以截图
void showObject(const cv::Mat& src, const std::vector<cv::Point2f>& area) {
  using namespace cv;
  Mat& dst = object_dst;
  Mat bin;
  normalizePlate(src, dst, area);
  imshow("raw", dst);
  cvtColor(dst, bin, cv::COLOR_BGR2GRAY);
  threshold(bin, bin, 240, 0, cv::THRESH_TOZERO_INV);
  threshold(bin, bin, 0, 255, cv::THRESH_OTSU);
  imshow("dst", bin);
}

void click(int event, int, int, int, void*) {
  if (event != cv::EVENT_LBUTTONDOWN) return;
  auto path = std::string("captures/") + std::to_string(time(NULL)) + ".jpg";
  imwrite(path, object_dst);
  std::cout << path << std::endl;
}

// Calculates rotation matrix to euler angles
// The result is the same as MATLAB except the order
// of the euler angles ( x and z are swapped ).
cv::Vec3f rotationMatrixToEulerAngles(cv::Mat& R) {
  float sy = sqrt(R.at<double>(0, 0) * R.at<double>(0, 0) +
                  R.at<double>(1, 0) * R.at<double>(1, 0));
  bool singular = sy < 1e-6;  // If
  float x, y, z;
  if (!singular) {
    x = atan2(R.at<double>(2, 1), R.at<double>(2, 2));
    y = atan2(-R.at<double>(2, 0), sy);
    z = atan2(R.at<double>(1, 0), R.at<double>(0, 0));
  } else {
    x = atan2(-R.at<double>(1, 2), R.at<double>(1, 1));
    y = atan2(-R.at<double>(2, 0), sy);
    z = 0;
  }
  return cv::Vec3f(x, y, z);
}

int main() {
  using namespace cv;

  cv::setUseOptimized(true);
  std::cin.sync_with_stdio(false);
  cv::Mat distortionCoefficents =
      (Mat_<float>(5, 1) << (float)-4.8782217469186462e-01,
       (float)3.6778476284229095e-01, (float)1.8386336284921304e-03,
       (float)-2.3476133476748938e-03, (float)-2.1742985016139418e-01);
  cv::Mat cameraMatrix =
      (Mat_<float>(3, 3) << (float)5.6642872233586195e+02, 0.,
       (float)3.3119547312606750e+02, 0., (float)5.6642872233586195e+02,
       (float)2.3690656334281314e+02, 0., 0., 1.);

  VideoCapture capture(0);
  if (!capture.isOpened()) {
    std::cout << "Can not open video" << std::endl;
    return 0;
  }

  loadExamples();

  capture.set(CAP_PROP_FRAME_WIDTH, 640);
  capture.set(CAP_PROP_FRAME_HEIGHT, 480);
  capture.set(CAP_PROP_EXPOSURE, -11);
  namedWindow("Source");
  namedWindow("dst");
  setMouseCallback("dst", click);

  RNG rng(6546531654);
  clock_t beginT, endT;
  for (;;) {
    beginT = clock();
    Mat src;
    if (!capture.read(src)) break;
    CV_Assert(src.data);
    // Image source
    Mat lab;
    cvtColor(src, lab, COLOR_BGR2Lab);
    Mat binary;
    inRange(lab, Scalar{240, 0, 128}, Scalar{255, 128, 255}, binary);
    std::vector<std::vector<Point>> contours;
    std::vector<RotatedRect> rrects;
    findContours(binary, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    for (auto&& contour : contours) {
      auto rect = minAreaRect(contour);
      if (rect.size.area() < 100) continue;
      rrects.push_back(rect);
      drawRotatedRect(src, rrects.back(), Scalar(200, 100, 200, 200));
    }
    std::vector<std::vector<Point2f>> plates;
    for (size_t i = 0; i < rrects.size(); i++) {
      for (size_t j = i + 1; j < rrects.size(); j++) {
        plates.push_back(getPlate(rrects[i], rrects[j]));
        showObject(src, plates.back());
      }
    }
    std::vector<std::vector<Point2f>> true_plates;
    std::copy_if(plates.cbegin(), plates.cend(),
                 std::back_inserter(true_plates), [&src](const auto& plate) {
                   Mat dst;
                   normalizePlate(src, dst, plate);
                   cvtColor(dst, dst, cv::COLOR_BGR2GRAY);
                   threshold(dst, dst, 240, 0, cv::THRESH_TOZERO_INV);
                   threshold(dst, dst, 0, 255, cv::THRESH_OTSU);
                   return guessArmor(dst);
                 });
    double w = 1, h = 1;
    std::vector<Point3d> tmp;
    tmp.push_back({-w, h, 0});
    tmp.push_back({-w, -h, 0});
    tmp.push_back({w, -h, 0});
    tmp.push_back({w, h, 0});
    std::vector<Point3f> ref_obj{{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, -1}};
    float distance, yaw, pitch;
    for (auto&& p : true_plates) {
      Mat rVec, tVec;
      drawBox(src, p, {200, 100, 100});
      solvePnP(tmp, p, cameraMatrix, distortionCoefficents, rVec, tVec);
      // Mat rotM;
      // cv::Rodrigues(rVec, rotM);
      // Vec3f angle = rotationMatrixToEulerAngles(rotM);
      Point2f center;
      center.x = (p[0].x + p[2].x) / 2.0;
      center.y = (p[0].y + p[2].y) / 2.0;
      distance = norm(tVec);
      pitch =-57.3 * atan2(tVec.at<double>(0, 1), tVec.at<double>(0, 2));
      yaw = 57.3 * atan2(tVec.at<double>(0, 0), tVec.at<double>(0, 2)) ;
      std::vector<Point2f> ref_img;
      projectPoints(ref_obj, rVec, tVec, cameraMatrix, distortionCoefficents,
                    ref_img);
      line(src, ref_img[0], ref_img[1], {0, 0, 255}, 3);
      line(src, ref_img[0], ref_img[2], {0, 255, 0}, 3);
      line(src, ref_img[0], ref_img[3], {255, 0, 0}, 3);
    }
    if (true_plates.size() == 1) showObject(src, true_plates[0]);

    std::stringstream ss;
    endT = clock();
    ss << "fps: " << 1000 / (1000 * (endT - beginT) / CLOCKS_PER_SEC);
    putText(src, ss.str(), {0, 50}, FONT_HERSHEY_PLAIN, 3, {255, 255, 255},
            3);
    putText(src, std::string("dist: ") + std::to_string(distance), {0, 100}, FONT_HERSHEY_PLAIN, 3, {255,255,255}, 3);
    putText(src, std::string("yaw: ") + std::to_string(yaw), {0, 150}, FONT_HERSHEY_PLAIN, 3, {255,255,255}, 3);
    putText(src, std::string("pitch: ") + std::to_string(pitch), {0, 200}, FONT_HERSHEY_PLAIN, 3, {255,255,255}, 3);
    imshow("Source", src);
    if (waitKey(1) != -1) break;
    // waitKey();
  }
  return 0;
}
