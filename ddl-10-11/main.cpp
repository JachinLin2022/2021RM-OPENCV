#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
using namespace std;
using namespace cv;

int main()
{
    Mat src = imread("test.jpg");
    if (!src.data)
    {
        cout << "image can't be opend" << endl;
        return -1;
    }
    else
        cout << "image open" << endl;
    Mat HSV;
    Mat binary = src.clone();
    //RGB To HSV
    cvtColor(src, HSV, CV_RGB2HSV);

    /*Method 1 and method 2have no difference in the image output
when the parameters are the same.
  Using two methods to complete this is only for validation*/

    // method 1: using inRange
    cv::Scalar scalarL = cv::Scalar(100, 43, 250);
    cv::Scalar scalarH = cv::Scalar(105, 255, 255);
    inRange(HSV, scalarL, scalarH, binary);
    imwrite("gray.jpg", binary);

    //method 2: not using inRange
    // double H=0,S=0,V=0;
    // for(int i=0;i<HSV.rows;i++)
    // {
    //     for(int j=0;j<HSV.cols;j++)
    //     {
    //         H=HSV.at<Vec3b>(i,j)[0];
    //         S=HSV.at<Vec3b>(i,j)[1];
    //         V=HSV.at<Vec3b>(i,j)[2];
    //         if(H>=100&&H<=105)
    //         {
    //             if(S>=43&&S<=255&&V>=250&&V<=255)
    //             {
    //                 binary.at<Vec3b>(i,j)[0]=255;
    //                 binary.at<Vec3b>(i,j)[1]=255;
    //                 binary.at<Vec3b>(i,j)[2]=255;
    //             }
    //             else
    //             {
    //                 binary.at<Vec3b>(i,j)[0]=0;
    //                 binary.at<Vec3b>(i,j)[1]=0;
    //                 binary.at<Vec3b>(i,j)[2]=0;
    //             }
    //         }
    //         else
    //         {
    //             binary.at<Vec3b>(i,j)[0]=0;
    //             binary.at<Vec3b>(i,j)[1]=0;
    //             binary.at<Vec3b>(i,j)[2]=0;
    //         }
    //     }
    // }
    // imwrite("binary.jpg",binary);

    waitKey(0);

    return 0;
}
