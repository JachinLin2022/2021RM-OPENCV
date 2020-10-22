#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

int main(){
    /* Inherited from task 2, where we are requested to binarize the image and extract the light bars. */


    cv::Mat srcImage = cv::imread("test.jpg"), HSVImage, processedImage;
    cv::cvtColor(srcImage, HSVImage, cv::COLOR_BGR2HSV);
    cv::inRange(HSVImage, cv::Scalar(0, 43, 250),\
			    cv::Scalar(30, 255, 255), processedImage);

    /* Get the contours from the processed image. */

    std::vector < std::vector <cv::Point> > contours;
    cv::Mat imageContours=cv::Mat::zeros(processedImage.size(),CV_8UC1);

    std::vector <cv::Vec4i> hierarchy;
    cv::findContours(processedImage, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE, cv::Point());
    
    cv::drawContours(imageContours, contours, -1, cv::Scalar(255), 1, 8, hierarchy);
    cv::imwrite("contour.jpg", imageContours);

    /* Get Rotated Rectangles from the contours calculated. */
}