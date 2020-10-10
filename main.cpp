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

    //显示原图像
   namedWindow("原图像",WINDOW_AUTOSIZE);
   imshow("原图像",srcImage);

   //将图像转换为HSV图,采用COLOR_前缀
   Mat HSVImage;
   Mat mask;
   cvtColor(srcImage, HSVImage, COLOR_BGR2HSV);

    //将图像二值化
   Scalar scalarL = Scalar(6,115,85);
   Scalar scalarH = Scalar(10,255,255);
   inRange(HSVImage,scalarL,scalarH,mask);
   namedWindow("二值化",WINDOW_AUTOSIZE);
   imshow("二值化",mask);
   imwrite("gray.jpg",mask);

   waitKey(0);
   
   return 0;
}