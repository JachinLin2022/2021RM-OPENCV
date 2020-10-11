#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

int main()
{
   Mat srcImage = imread("test.jpg");

   //判断图像是否加载成功
   if(!srcImage.data){
       cout << "图像加载失败!" << endl;
       return false;
   } 
   else
       cout << "图像加载成功!" << endl << endl;

   //将图像转换为HSV图,采用COLOR_前缀
   Mat HSVImage,mask;

   cvtColor(srcImage, HSVImage, COLOR_BGR2HSV);

    //将图像二值化
   inRange(HSVImage,Scalar(0,240,130),Scalar(10,255,255),mask);
   namedWindow("gray",WINDOW_AUTOSIZE);
   imshow("gray",mask);
   imwrite("gray.jpg",mask);

   waitKey(0);

   return 0;
} 
