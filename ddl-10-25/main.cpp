#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include<opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc.hpp>
#include<math.h>
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
    Mat hsv;
    Mat binary;
    //RGB To HSV
    cvtColor(src, hsv, COLOR_RGB2HSV);

    //(1):binaryzation
    cv::Scalar scalarL = cv::Scalar(100, 43, 245);
    cv::Scalar scalarH = cv::Scalar(110, 255, 255);
    inRange(hsv, scalarL, scalarH, binary);

    //(2) and (3):draw contours and minimun rectangel
    vector<vector<Point>>contours;
    vector<Vec4i> hierarchy;
    findContours(binary,contours,hierarchy,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE,Point());
    Mat contour_image=Mat::zeros(binary.size(),CV_8U);
    namedWindow("contour",CV_WINDOW_NORMAL);
    vector<RotatedRect> minRect(contours.size());

    Mat armor=src.clone();
    for(int i=0;i<contours.size();i++)
    {
        if(i==35||i==41||i==43||i==48||i==49||i==63||i==69)
        { 
            drawContours(contour_image,contours,i,Scalar(255,255,255));
            if(i!=43)
            {
                minRect[i]=minAreaRect(Mat(contours[i]));
                Point2f rect_points[4];
                minRect[i].points(rect_points);
                for(int j=0;j<4;j++)
                    line(contour_image,rect_points[j],rect_points[(j+1)%4],cv::Scalar(205,90,106),4,8,0);
            }
        }
    }

    //(4) and (5) and (6):mark and match the paired bars and frame out armor plate
    int i_contour[]={35,41,48,49,63,69};
    for(int i=0;i<6;i++)
    {
        for(int j=i+1;j<6;j++)
        {
            if(i_contour[i]&&i_contour[j])
            {
                if(fabs(minRect[i_contour[i]].angle-minRect[i_contour[j]].angle)<1)
                {
                    Point2f rect_points_1[4];
                    Point2f rect_points_2[4];
                    minRect[i_contour[i]].points(rect_points_1);
                    minRect[i_contour[j]].points(rect_points_2);
                    Point2f rect_points[4];
                    int length=100;
                    double angle_1,angle_2,angle;
                    angle_1=minRect[i_contour[i]].angle;
                    angle_2=minRect[i_contour[j]].angle;
                    if(fabs(sin(angle_1))<fabs(sin(angle_2)))
                        angle=angle_1;
                    else
                        angle=angle_2;
                    rect_points[0].x=minRect[i_contour[i]].center.x-fabs(length*sin(angle));
                    rect_points[0].y=minRect[i_contour[i]].center.y-fabs(length*cos(angle));
                    rect_points[1].x=minRect[i_contour[i]].center.x+fabs(length*sin(angle));
                    rect_points[1].y=minRect[i_contour[i]].center.y+fabs(length*cos(angle));
                    rect_points[2].x=minRect[i_contour[j]].center.x+fabs(length*sin(angle));
                    rect_points[2].y=minRect[i_contour[j]].center.y+fabs(length*cos(angle));
                    rect_points[3].x=minRect[i_contour[j]].center.x-fabs(length*sin(angle));
                    rect_points[3].y=minRect[i_contour[j]].center.y-fabs(length*cos(angle));
                    for(int k=0;k<4;k++)
                    {
                        line(armor,rect_points_1[k],rect_points_1[(k+1)%4],cv::Scalar(205,90,106),5,8,0);
                        line(armor,rect_points_2[k],rect_points_2[(k+1)%4],cv::Scalar(205,90,106),5,8,0);
                        line(armor,rect_points[k],rect_points[(k+1)%4],cv::Scalar(201,252,189),5,8,0);
                    }
                    line(armor,minRect[i_contour[i]].center,minRect[i_contour[j]].center,cv::Scalar(205,90,106),5,8,0);

                    i_contour[i]=0;
                    i_contour[j]=0;
                }
            }
        }
    }

    imwrite("armor.jpg",armor);

    return 0;
}