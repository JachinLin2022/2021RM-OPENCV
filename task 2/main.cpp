#include <opencv2/opencv.hpp>

/* @name filterRedLight
 * @param Source image in BGR
 * @param Likeliness of red light, as 8UC1 matrix.
 * @brief (Contour of) red light = very low hue + very high lightness.
 * @note The contour has high hue, while the body area of light is almost white.
 */
void filterRedLight(const cv::Mat& src, cv::Mat& dst);

/* @name gammaCorrection
 * @brief perform gamma correction on src, values lower than black are flushed
 * to 0, higher than white are flushed to 255
 */
void gammaCorrection(const cv::Mat& src, double gamma, double black,
                     double white, cv::Mat& dst);

const static double kGamma = 3, kBlack = 180, kWhite = 245, kThreshold = 200;

int main() {
  using namespace cv;
  Mat src = imread("test.jpg");
  CV_Assert(src.data);

  Mat grey;
  filterRedLight(src, grey);
  imwrite("grey.png", grey);

  return 0;
}


void filterRedLight(const cv::Mat& src, cv::Mat& dst) {
  using namespace cv;
  CV_Assert(src.type() == CV_8UC3);
  // HSV image
  Mat hsv;
  cvtColor(src, hsv, CV_BGR2HSV);
  // Value channel of hsv
  Mat value;
  extractChannel(hsv, value, 2);
  // The image is highly overexposed, correct it with gamma correction
  gammaCorrection(value, kGamma, kBlack, kWhite, dst);

  // Apply threshold and 
  auto si = hsv.begin<Vec3b>();
  auto send = hsv.end<Vec3b>();
  auto di = dst.begin<uchar>();
  for (; si != send; ++si, ++di) {
    if (*di < 200) *di = 0;
    *di = saturate_cast<uchar>(unsigned(*di) * unsigned(~(*si)[0]));
  }
}

void gammaCorrection(const cv::Mat& src, double gamma, double black,
                     double white, cv::Mat& dst) {
  cv::Mat look_up_table(1, 256, CV_8UC1);
  uchar* p = look_up_table.ptr();
  for (int i = 0; i < 256; i++) {
    if (i < black) {
      p[i] = 0;
    } else if (i > white) {
      p[i] = 255;
    } else {
      p[i] = cv::saturate_cast<uchar>(
          pow((i - black) / (white - black), gamma) * 255.0);
    }
  }
  cv::LUT(src, look_up_table, dst);
}