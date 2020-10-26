#include <iostream>
#include "opencv2/opencv.hpp"

int main(int argc,char **argv){
	cv::VideoCapture capture(0);

	cv::VideoWriter out("test.avi",0,20.0,cv::Size(640,480));
	if(!capture.isOpened()){
		std::cerr<<"Error when turing on the camera."<<std::endl;
		return -1;
	}

	cv::Mat Frame;
	while(capture.read(Frame)){
		std::size_t rowCount = Frame.isContinuous()?1:Frame.rows;
		std::size_t columnCount = Frame.isContinuous()?Frame.cols*Frame.rows*Frame.channels():
			Frame.cols*Frame.channels();
		for(int i=0;i<rowCount;++i){
			const auto rowPointer = Frame.ptr<uchar>(i);
			for(int j=0;j<columnCount;++j)
				rowPointer[j]=255-rowPointer[j];
		}
		out << Frame;
	}
	out.release();
	return 0;
}
