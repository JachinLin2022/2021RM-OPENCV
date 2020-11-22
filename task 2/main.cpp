#include <iostream>
#include <opencv2\opencv.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

int main() 
{
	//namedWindow("test_image", CV_WINDOW_AUTOSIZE);
	//namedWindow("gray_image", CV_WINDOW_AUTOSIZE);
	Mat RGB_src = imread("test.jpg");
	imshow("原始图", RGB_src);
	//waitKey(0);
	Mat HSV_image;
	cvtColor(RGB_src,HSV_image,COLOR_BGR2HSV);
	//show("HSV图", HSV_image);
	//itKey(0);

	//define threshold
	int low_H = 0;
	int high_H = 0.1;
	int low_S = 5;
	int high_S = 30;
	int low_V = 45;
	int high_V = 180;
	
	//define output image
	Mat gray_image = RGB_src.clone();

	//filt
	inRange(RGB_src, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), gray_image);

	//output
	imshow("gray image", gray_image);
	waitKey(0);	

	return(0);
}
