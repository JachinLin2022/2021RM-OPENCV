#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

int main(){
    /* Inherited from task 2, where we are requested to binarize the image and extract the light bars. */
    cv::Mat srcImage = cv::imread("test.jpg"), HSVImage, processedImage;
    cv::cvtColor(srcImage, HSVImage, cv::COLOR_BGR2HSV);
    cv::inRange(HSVImage, cv::Scalar(0, 43, 248),\
			    cv::Scalar(30, 255, 255), processedImage);

    /* Get the contours from the processed image. */
    std::vector < std::vector <cv::Point> > contours;
    cv::Mat imageContours=cv::Mat::zeros(processedImage.size(),CV_8UC1);
    std::vector <cv::Vec4i> hierarchy;
    cv::findContours(processedImage, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE, cv::Point());
    cv::drawContours(imageContours, contours, -1, cv::Scalar(255), 1, 8, hierarchy);
    
    /* Get Rotated Rectangles from the contours calculated. */
    std::vector <cv::RotatedRect> rotate_rect;
    for(auto contoursIter=contours.begin(); contoursIter!=contours.end(); ++contoursIter)
        if(cv::contourArea(*contoursIter)>250.0){
            rotate_rect.push_back(cv::minAreaRect(*contoursIter));
            cv::Point2f* vertices = new cv::Point2f[4];
	        rotate_rect.back().points(vertices);
    	    for (int j = 0; j < 4; j++){
	    	    cv::line(srcImage, vertices[j], vertices[(j + 1) % 4], cv::Scalar(0, 255, 0));
     	    }
    }

    cv::imwrite("armor.png",srcImage);
    return 0;
}
