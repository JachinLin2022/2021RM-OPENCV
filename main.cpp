#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include<math.h>

using namespace std;
using namespace cv;

int main()
{
    Mat srcImage = cv::imread("test.jpg");
    if (!srcImage.data)
    {
        std::cout << "图像加载失败!" << std::endl;
        return false;
    }
    else
        std::cout << "图像加载成功!" << std::endl << std::endl;
    // namedWindow("原图像", cv::WINDOW_AUTOSIZE);
    // imshow("原图像", srcImage);

     //BGR to  HSV
    Mat HSVImage;
    Mat binaryImage;
    cvtColor(srcImage, HSVImage, COLOR_BGR2HSV);

    //binary
    cv::Scalar scalarL = cv::Scalar(0, 43, 245);
    cv::Scalar scalarH = cv::Scalar(25, 255, 255);
    inRange(HSVImage, scalarL, scalarH, binaryImage);
    namedWindow("二值化", cv::WINDOW_AUTOSIZE);
    imshow("二值化", binaryImage);

    //find contours
    cv::Mat rectImage = srcImage.clone();
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hireachy;
    findContours(binaryImage, contours, hireachy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
    //Mat conImage = srcImage.clone();
    Point2f centrPoint[10];
    Point2f amorPoints[4];
    double angles[10];
    int BARnum = 0;

    for (int i = 0; i < (int)contours.size(); i++)
    {
        RotatedRect rect = cv::minAreaRect(contours[i]);//为每个轮廓定义一个rRect
       //Point2f* vertices = new cv::Point2f[4];//定义四个点
        Point2f vertices[4];
        rect.points(vertices);//初始化
        double height = (rect.size.height > rect.size.width) ? rect.size.height : rect.size.width;
        double width = (rect.size.height > rect.size.width) ? rect.size.width : rect.size.height;
        double n = height / width;

        //根据矩形长宽比和长度筛选矩形
        if (n >= 2.6 && height >= 18 && height <= 30)
        {
            drawContours(srcImage, contours, -1, cv::Scalar(255, 0, 0), 2, 8);
            for (int k = 0; k < 4; k++)
            {
                line(rectImage, vertices[k], vertices[(k + 1) % 4], Scalar(0, 255, 0), 2);
                angles[BARnum] = rect.angle;
            }
            centrPoint[BARnum] = rect.center;
            BARnum = BARnum + 1;
        }
    }
    cv::imshow("灯条识别", rectImage);


    //bar match
    //parallel and distance
    float distance;
    const float sidelength = 50;
    for (int m = 0; m < BARnum; m++)
    {
        for (int n = 0; n <= m; n++)
        {
            distance = sqrt(pow((centrPoint[m].x - centrPoint[n].x), 2) + pow((centrPoint[m].y - centrPoint[n].y), 2));
            if (distance > 30 && distance < 70)
            {
                double angle = min(angles[n], angles[m]);
                Point s;
                s.x = (centrPoint[n].x + centrPoint[m].x) / 2;
                s.y = (centrPoint[n].y + centrPoint[m].y) / 2;
                RotatedRect armorRec = RotatedRect(Point2f(s.x, s.y), Size2f(60, 60), angle);
                Point2f armorPoint[4];
                armorRec.points(armorPoint);
                for (size_t k = 0; k < 4; k++)
                {
                    line(rectImage, armorPoint[k], armorPoint[(k + 1) % 4], Scalar(255, 0, 0), 4, 8, 0);
                }
                line(rectImage, centrPoint[m], centrPoint[n], Scalar(0, 0, 255), 4, 8, 0);
            }
        }
    }

    namedWindow("armor", cv::WINDOW_AUTOSIZE);
    imshow("armor", rectImage);
    imwrite("armor.jpg", rectImage);
    waitKey(0);

    return 0;
}
