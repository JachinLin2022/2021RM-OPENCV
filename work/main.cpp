


#include<iostream>
#include<opencv2/opencv.hpp>
using namespace cv;
int main()
{
    Mat image;
    VideoCapture capture(0);   //get image from camera
    capture.open(0);
    if(!capture.isOpened())
        return 0;
    while(1){
        Mat frame;
        capture>>frame;        //read current frame
        if(frame.empty())
        {
            break;
        }
        else
        {
            cvtColor(frame,image,CV_BGR2GRAY);
            cv::imwrite("work1_get.jpg",image);
            Mat img=cv::imread("work1_get.jpg",cv::IMREAD_GRAYSCALE);
            std:: cout<<img<<std::endl;
            break;
        }
        waitKey(30);
    }
    return 0;
}
