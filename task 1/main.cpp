#include <opencv2/opencv.hpp>

void invert_color(cv::Mat&);

int main() {
  cv::namedWindow("OpenCVCamera", cv::WINDOW_AUTOSIZE);
  cv::VideoCapture capture(0);
  {
    cv::Mat frame;
    capture >> frame;
    std::cout << "Type of frame: " << frame.type() << std::endl;
  }
  while (true) {
    cv::Mat frame;
    capture >> frame;
    invert_color(frame);
    cv::imshow("OpenCVCamera", frame);
    if (cv::waitKey(30) != -1) break;
  }
}

void invert_color(cv::Mat& mat) {
  assert(mat.depth() == CV_8U && mat.channels() == 3);
  // Get # of rows and cols
  int row_count = mat.isContinuous() ? 1 : mat.rows;
  auto i = mat.begin<cv::Vec3b>();
  auto end = mat.end<cv::Vec3b>();
  for (; i != end; i++) {
    auto&& v = *i;
    v[0] = ~v[0];
    v[1] = ~v[1];
    v[2] = ~v[2];
  }
}
