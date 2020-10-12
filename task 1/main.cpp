#include <opencv2/opencv.hpp>

int main() {
  cv::namedWindow("OpenCVCamera", cv::WINDOW_AUTOSIZE);
  cv::VideoCapture capture(0);

  while (true) {
    cv::Mat frame;
    capture >> frame;
    cv::imshow("OpenCVCamera", frame);
    if (cv::waitKey(30) != -1) break;
  }
}