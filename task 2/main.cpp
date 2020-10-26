#include <iostream>
#include <opencv2/opencv.hpp>

int main(){
    cv::Mat srcImage = cv::imread("test.jpg"), HSVImage, processedImage;
    cv::cvtColor(srcImage, HSVImage, cv::COLOR_BGR2HSV);
    cv::inRange(HSVImage, cv::Scalar(0, 43, 250),\
			    cv::Scalar(30, 255, 255), processedImage);
    cv::imwrite("gray.jpg", processedImage);
    return 0;
}
