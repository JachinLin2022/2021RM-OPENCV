#include<opencv2/opencv.hpp>

using namespace cv;

int main(){
        cv::namedWindow("OpenCVCamera",cv::WINDOW_AUTOSIZE);
        cv::VideoCapture capture(0);

        while(true){
                cv::Mat frame,image;
                capture>>frame;

                image.create(frame.size(),frame.type());

                int height=frame.rows;
                int width=frame.cols;

                for(int i=0;i<height;i++){
                        for(int j=0;j<width;j++){
                                image.at<cv::Vec3b>(i,j)[0]=255-frame.at<cv::Vec3b>(i,j)[0];
                                image.at<cv::Vec3b>(i,j)[1]=255-frame.at<cv::Vec3b>(i,j)[1];
                                image.at<cv::Vec3b>(i,j)[2]=255-frame.at<cv::Vec3b>(i,j)[2];
                        }
                }

                cv::imshow("OpenCVCamera",image);
                if(cv::waitKey(30)!=-1){break;}
        }

        return 0;
}
