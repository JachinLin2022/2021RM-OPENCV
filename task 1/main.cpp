#include <opencv2/opencv.hpp>
/**@name invertColorIterator
 * @brief Invert the colors with cv::Mat's iterator interface
 */
void invertColorIterator(cv::Mat&);

/**@name invert color_ptr
 * @brief Invert the colors with pointers, the approach described in ppt
 */
void invertColorPointer(cv::Mat&);

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
    invertColorIterator(frame);
    cv::imshow("OpenCVCamera", frame);
    if (cv::waitKey(30) != -1) break;
  }
}

void invertColorIterator(cv::Mat& mat) {
  CV_Assert(mat.type() == CV_8UC3);
  
  auto i = mat.begin<cv::Vec3b>();
  auto end = mat.end<cv::Vec3b>();
  for (; i != end; i++) {
    auto&& v = *i;
    v[0] = ~v[0];
    v[1] = ~v[1];
    v[2] = ~v[2];
  }
}

void invertColorPointer(cv::Mat& mat) {
  CV_Assert(mat.depth() == CV_8U);
  // # of rows and cols
  int row_count = mat.isContinuous() ? 1 : mat.rows;
  int col_count = mat.isContinuous() ? mat.rows * mat.cols * mat.channels()
                                     : mat.cols * mat.channels();
  for (int i = 0; i < row_count; ++i) {
    auto row_ptr = mat.ptr<uchar>(i);
    for (int j = 0; j < col_count; ++j) {
      row_ptr[j] = ~row_ptr[j];
    }
  }
}
