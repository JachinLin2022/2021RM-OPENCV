#include <iostream>
//#include<stdio.h>
#include <opencv2\opencv.hpp>
#include<opencv2/imgproc/imgproc.hpp>

using namespace cv;

int main() {
	VideoCapture capture(0);
	while (1) {
		Mat image;
		capture >> image;
		int rowNumbers = image.rows;
		int colNumbers = image.cols*3;
		for (int i = 0; i < rowNumbers; i++)
		{
			uchar* data = image.ptr<uchar>(i);
			for (int j = 0; j < colNumbers; j++) {
				int curData = data[j];
				data[j] = 255 - curData;
			}	
		}
		imshow("image",image);
		waitKey(30);
		
	}
	return(0);
}
