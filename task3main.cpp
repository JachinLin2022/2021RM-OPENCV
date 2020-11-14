#include <cmath>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

bool isLightBar(cv::RotatedRect rect){
    cv::Point2f* vertices = new cv::Point2f[4];
    rect.points(vertices);
    double distA=sqrtf(powf(vertices[0].x-vertices[1].x,2)+powf(vertices[0].y-vertices[1].y,2));
    double distB=sqrtf(powf(vertices[0].x-vertices[3].x,2)+powf(vertices[0].y-vertices[3].y,2));
    delete vertices;
    return (((distA/distB)>2.7)||((distA/distB)<1.0/2.7))&&distA*distB>=1000.0;
}

bool isParallel(cv::RotatedRect lhs,cv::RotatedRect rhs){
    return abs(lhs.angle-rhs.angle)<5;
}

double Dist(cv::RotatedRect lhs,cv::RotatedRect rhs){
    return sqrt(pow(lhs.center.x-rhs.center.x,2)+pow(lhs.center.y-rhs.center.y,2));
}

int main(){
    /* Inherited from task 2, where we are requested to binarize the image and extract the light bars. */
    cv::Mat srcImage = cv::imread("test.jpg"), HSVImage, processedImage;
    cv::cvtColor(srcImage, HSVImage, cv::COLOR_BGR2HSV);
    cv::inRange(HSVImage, cv::Scalar(10, 43, 240),\
			    cv::Scalar(30, 250, 255), processedImage);
    std::cout << "Have Passed Problem 1" << std::endl;

    /* Get the contours from the processed image. */
    std::vector < std::vector <cv::Point> > contours;
    cv::Mat imageContours=cv::Mat::zeros(processedImage.size(),CV_8UC1);
    std::vector <cv::Vec4i> hierarchy;
    cv::findContours(processedImage, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE, cv::Point());
    std::cout << "Have Passed Problem 2" << std::endl;

    /* Get Rotated Rectangles from the contours calculated. */
    std::vector <cv::RotatedRect> rotate_rect;
    for(auto contoursIter=contours.begin(); contoursIter!=contours.end(); ++contoursIter)
        if(isLightBar(cv::minAreaRect(*contoursIter))){
            rotate_rect.push_back(cv::minAreaRect(*contoursIter));
            std::cout << cv::minAreaRect(*contoursIter).angle << std::endl;
        }

    /* Choose paired rectangles by comparing angles */
    for(auto rotateIter=rotate_rect.begin();rotateIter!=rotate_rect.end();++rotateIter)
        for(auto secondIter=rotateIter+1;secondIter!=rotate_rect.end();++secondIter)
            if(isParallel(*rotateIter,*secondIter)&&Dist(*rotateIter,*secondIter)<=300.0){
                /* Print the Midpoint of the rectangles. */
                cv::circle(srcImage, rotateIter->center, 10, cv::Scalar(255,0,0),-1);
                cv::circle(srcImage, secondIter->center, 10, cv::Scalar(255,0,0),-1);
                /* Print the Parallel Rectangles. */
                cv::Point2f* verticesA = new cv::Point2f[4];
                cv::Point2f* verticesB=new cv::Point2f[4];
                rotateIter->points(verticesA);
                for (size_t i = 0; i < 4; i++)
                    cv::line(srcImage, verticesA[i], verticesA[(i + 1) % 4], cv::Scalar(0, 255, 0), 4, 8, 0);
                secondIter->points(verticesB);
                for (size_t i = 0; i < 4; i++)
                    cv::line(srcImage, verticesB[i], verticesB[(i + 1) % 4], cv::Scalar(0, 255, 0), 4, 8, 0);
                /* Find the armor */
                
                cv::line(srcImage, verticesA[2], verticesB[1], cv::Scalar(0, 255, 0), 4, 8, 0);
                cv::line(srcImage, verticesA[3], verticesB[0], cv::Scalar(0,255,0), 4, 8, 0);
            }
    
    /* Yattaze! 2020-10-25 01:22 */
    cv::imwrite("armor.jpg",srcImage);
    return 0;
}