#include <iostream>
#include<cmath>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

float distance(float a, float b) {
	return sqrt(pow(a, 2) + pow(b, 2));
}

int main()
{
	Mat srcImage = imread("test.jpg");

	//判断图像是否加载成功
	if (!srcImage.data) {
		cout << "图像加载失败!" << endl;
		return false;
	}
	else
		cout << "图像加载成功!" << endl << endl;

	//将图像转换为HSV图,采用COLOR_前缀
	Mat HSVImage, mask;

	cvtColor(srcImage, HSVImage, COLOR_BGR2HSV);

	//将图像二值化
	inRange(HSVImage, Scalar(0, 43, 45), Scalar(10, 255, 255), mask);

	vector<vector<Point>> lightContours;
	//找轮廓
	findContours(mask.clone(), lightContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	//灯条拟合矩形
	vector<RotatedRect>  LightRect;

	for (const auto& contour : lightContours)
	{
		//得到面积
		float lightContourArea = contourArea(contour);
		//根据面积排除干扰
		if (contour.size() > 225 || contour.size() < 150) continue;
		//旋转矩形
		RotatedRect Rect = minAreaRect(contour);
		//根据宽高比排除干扰
		double height = max(Rect.size.width, Rect.size.height);
		double width = min(Rect.size.width, Rect.size.height);
		if (height / width > 6 || height / width < 2) {
			continue;
		}
		LightRect.push_back(Rect);
	}

	float dis;
	//遍历所有灯条进行匹配
	for (size_t i = 0; i < LightRect.size(); i++){
		for (size_t j = i + 1; (j < LightRect.size()); j++) {
			//根据是否平行匹配
			cv::Point2f* pointi = new cv::Point2f[4];
			cv::Point2f* pointj = new cv::Point2f[4];
			LightRect[i].points(pointi);
			LightRect[j].points(pointj);
			float k1 = (LightRect[i].center.y - pointi[0].y) / (LightRect[i].center.x - pointi[0].x);
			float k2 = (LightRect[j].center.y - pointj[0].y) / (LightRect[j].center.x - pointj[0].x);
			float kmax = max(k1, k2);
			float kmin = min(k1, k2);
			if (abs(kmax / kmin) > 1.5) {
				continue;
			}
			delete pointi;
			delete pointj;
			float angle = abs(LightRect[i].angle - LightRect[j].angle);
			if (angle > 10) {
				continue;
			}
			//根据中心点距离匹配
			dis = distance((LightRect[i].center.x - LightRect[j].center.x), (LightRect[i].center.y - LightRect[j].center.y));
			if (dis < 400) {
				cv::Point2f* vertices = new cv::Point2f[4];
				LightRect[i].points(vertices);
				for (size_t i = 0; i < 4; i++) {
					cv::line(srcImage, vertices[i], vertices[(i + 1) % 4], cv::Scalar(0, 255, 0), 4, 8, 0);
				}
				LightRect[j].points(vertices);
				for (size_t i = 0; i < 4; i++) {
					cv::line(srcImage, vertices[i], vertices[(i + 1) % 4], cv::Scalar(0, 255, 0), 4, 8, 0);
				}
				cv::line(srcImage, LightRect[i].center, LightRect[j].center, cv::Scalar(0, 255, 0), 4, 8, 0);

				float k = (LightRect[i].center.y - LightRect[j].center.y) / (LightRect[i].center.x - LightRect[j].center.x);
				k = -1 / k;
				int a = 40;
				cv::Point2f* frame = new cv::Point2f[4];
				frame[0].x = LightRect[i].center.x - a;
				frame[0].y = LightRect[i].center.y - a * k;
				frame[3].x = LightRect[i].center.x + a;
				frame[3].y = LightRect[i].center.y + a * k;
				frame[1].x = LightRect[j].center.x - a;
				frame[1].y = LightRect[j].center.y - a * k;
				frame[2].x = LightRect[j].center.x + a;
				frame[2].y = LightRect[j].center.y + a * k;
				for (size_t i = 0; i < 4; i++) {
					cv::line(srcImage, frame[i], frame[(i + 1) % 4], cv::Scalar(0, 255, 0), 4, 8, 0);
				}
			}
		}
	}
	imwrite("armor.jpg", srcImage);
	return 0;
}