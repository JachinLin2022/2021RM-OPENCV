#include <opencv2/opencv.hpp>
#include <dirent.h>
#include <sys/types.h>

static std::vector<cv::Mat> numberExamples, blankExamples;
const static int _ACCEPTABLE_LOWER_BOUND = 1300;

/* int _compare(cv::Mat, cv::Mat)
 * This function is designed to compare the two different matrices.
 * The similarity of two matrices is represented by the 'sameScore'.
 */
int _compare(const cv::Mat& lhs, const cv::Mat& rhs) {
  uint32_t r = sample_size, c = sample_size;
  if (lhs.isContinuous() && rhs.isContinuous()) {
    c *= r;
    r = 1;
  }
  register int sameScore = 0;
  for (uint32_t i = 0; i < r; ++i) {
    const uchar* lhsPointer = lhs.ptr<uchar>(i);
    const uchar* rhsPointer = rhs.ptr<uchar>(i);
    for (uint32_t j = 0; j < c; ++j) {
      register bool leftValue = *lhsPointer++, rightValue = *rhsPointer++;
      if (!leftValue && !rightValue) continue;
      sameScore += (leftValue && rightValue) ? 2 : -1; 
    }
  }
  return sameScore;
}

/* bool guessArmor(cv::Mat)
 * This function is designed to determine whether a given picture is an armor
 * board or not. The designers assume that the mat has been binarized before the
 * function executes.
 */
bool guessArmor(const cv::Mat& mat) {
  // Resize the mat into a given size.
  cv::resize(mat, mat, cv::Size(sample_size, sample_size));
  int weightForNow, weightInAll = -2147483647;
  int pivot = 0, index = 0;
  for (; index < numberExamples.size(); ++index) {
    weightForNow = _compare(mat, numberExamples[index]);
    if (weightInAll < weightForNow) {
      weightInAll = weightForNow;
      pivot = index;
    }
  }
  // If none of the image in the numberExamples is similar enough to the given
  // image, then there's no need to do anything else.
  if (weightInAll < _ACCEPTABLE_LOWER_BOUND) return false;
  // If there exists data among the blankExamples more similar to the given
  // image than those among numberExamples, it's assumed that the given image
  // has no armors.
  for (int i = 0; i < blankExamples.size(); ++i) {
    weightForNow = _compare(mat, blankExamples[i]);
    if (weightInAll < weightForNow) return false;
  }
  return true;
}

void loadExamples() {
  using namespace cv;DIR *dir;
  struct dirent *entry;
  if((dir=opendir("./captures/"))==NULL){
     perror("opendir() error");
     exit(0);
  }
  char addBuf[300];
  while(entry=readdir(dir)){
    strcpy(addBuf,"./captures/");
    if(strcmp(".",entry->d_name)==0||strcmp("..",entry->d_name)==0)
       continue;
    strcat(addBuf,entry->d_name);
    Mat img = imread(addBuf);
    cvtColor(img, img, cv::COLOR_BGR2GRAY);
    resize(img, img, Size(sample_size, sample_size));
    threshold(img, img, 200, 0, cv::THRESH_TOZERO_INV);
    threshold(img, img, 0, 255, cv::THRESH_OTSU);
    numberExamples.push_back(img);
    imshow("sample", img);
  }
}
