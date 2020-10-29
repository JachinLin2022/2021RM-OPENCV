#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

int main()
{
    cv::Mat srcImage = cv::imread("test.jpg");

    //判断图像是否加载成功
    if(!srcImage.data)
    {
        std::cout << "图像加载失败!" << std::endl;
        return false;
    } 
    else
        std::cout << "图像加载成功!" << std::endl << std::endl;

    //显示原图像
    namedWindow("原图像", cv::WINDOW_AUTOSIZE);
    imshow("原图像", srcImage);

    //将图像转换为HSV图,采用COLOR_前缀
    cv::Mat HSVImage;
    cv::Mat mask;
    cvtColor(srcImage, HSVImage, cv::COLOR_BGR2HSV);

    //将图像二值化
    cv::Scalar scalarL = cv::Scalar(0, 43, 245);
    cv::Scalar scalarH = cv::Scalar(25, 255, 255);
    inRange(HSVImage, scalarL, scalarH, mask);
    namedWindow("二值化", cv::WINDOW_AUTOSIZE);
    cv::imshow("二值化", mask);
    imwrite("gray.jpg", mask);

    //轮廓提取与矩形拟合
    cv::Mat rectImage = srcImage.clone();
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hireachy;
    findContours(mask, contours, hireachy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0,0));
    cv::Point2f points[24];
    cv::Point2f centers[6];
    double angles[6];
    cv::Point2f endpoints[4];

    int j = 0; 
    for (int i = 0; i < (int)contours.size(); i++)
    {
        cv::RotatedRect rRect = cv::minAreaRect(contours[i]);
        cv::Point2f* vertices = new cv::Point2f[4];
        rRect.points(vertices);
        double height = (rRect.size.height > rRect.size.width) ? rRect.size.height : rRect.size.width;
        double width = (rRect.size.height > rRect.size.width) ? rRect.size.width : rRect.size.height;
        double n = height / width;

        //根据矩形长宽比和长度筛选矩形
        if (n >= 2.6 && height >= 90 && height <= 110)
        {
            drawContours(srcImage, contours, -1, cv::Scalar(255), 2, 8);                  
            for (size_t k=0;k<4;k++)
            {
                cv::line(rectImage, vertices[k], vertices[(k+1)%4], cv::Scalar(0,255,0), 4, 8, 0);    
                angles[j] = (rRect.size.height > rRect.size.width) ? (90 + rRect.angle): (rRect.angle);
            }
            centers[j] = rRect.center;
            j=j+1;         
        }   
    }
    //灯条配对及装甲板标定
    float distance ;
    const float sidelength = 220;
    for (int m = 0; m < j; m++) 
    {
        for(int n = 0;n <= m;n++)
        {
            distance = sqrt(pow((centers[m].x - centers[n].x),2) + pow((centers[m].y - centers[n].y),2));
            if (distance > sidelength - 50 && distance < sidelength + 50 && angles[m] <= angles[n]+2 && angles[m] >= angles[n]-2)
            {  
                double angle = std::max(angles[n],angles[m]);
                endpoints[0].x=centers[m].x + fabs(sidelength*sin(angle)/2);
                endpoints[0].y=centers[m].y + fabs(sidelength*cos(angle)/2);
                endpoints[1].x=centers[n].x + fabs(sidelength*sin(angle)/2);
                endpoints[1].y=centers[n].y + fabs(sidelength*cos(angle)/2);
                endpoints[2].x=centers[n].x - fabs(sidelength*sin(angle)/2);
                endpoints[2].y=centers[n].y - fabs(sidelength*cos(angle)/2);
                endpoints[3].x=centers[m].x - fabs(sidelength*sin(angle)/2);
                endpoints[3].y=centers[m].y - fabs(sidelength*cos(angle)/2);
                for (size_t k=0;k<4;k++)
                {
                    cv::line(rectImage, endpoints[k], endpoints[(k+1)%4], cv::Scalar(255,0,0), 4, 8, 0);
                }
                cv::line(rectImage, centers[m], centers[n], cv::Scalar(0,0,255), 4, 8, 0);
            }
        }
    }

    namedWindow("armor", cv::WINDOW_AUTOSIZE);
    imshow("armor", rectImage);
    imwrite("armor.jpg", rectImage);
    cv::waitKey(0);

    return 0;
}