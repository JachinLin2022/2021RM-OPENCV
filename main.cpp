/*作者：金湘雯 王晨燕 
 *修改者：
 *最新更新日期：2020.11.17
 *文件内容描述：定义了Armor结构体和timer类，分别用来记录armor的信息和运行时间信息；
				定义了kbhit函数，功能是检测是否有按键按下，若有则返回1，否则返回0；
				定义了Draw_Contours_MinRect函数，功能是在二值化的图像上画灯条轮廓和最小矩形；
				定义了MarchBars_FrameOutArmor函数，功能是匹配灯条以及将匹配灯条间的装甲板信息储存；
				定义了Get_Armor函数，功能是截取出原图中的匹配灯条间的装甲板图片；
				定义了Classifier_Pnp函数，功能是实现分类器和pnp功能，并框出正确的装甲板，显示pnp信息；
 *文件功能描述：实现了对摄像头采集到的视频内容进行逐帧分析，框出其中正确的装甲板并显示其pnp信息，同时显示帧率和灯条数目以及正确的装甲板数目；
 *与其他文件的联系：包含了classifier.hpp文件，运用Armor文件夹中的样本做为分类器的标准，调用了camera.yml文件
 */
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc.hpp>
#include <math.h>
#include <chrono>
#include <future>  // Use std::async => MultiThreading
#include "classifier.hpp"
#include <linux/videodev2.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <stdio.h>


#define SCALAR_LOW_RED cv::Scalar(100, 43, 245)
#define SCALAR_HIGH_RED cv::Scalar(110, 255, 255)
#define SCALAR_LOW_BLUE cv::Scalar(0,43,245)
#define SCALAR_HIGH_BLUE cv::Scalar(50,255,255)
#define DRAWER_COLOR cv::Scalar(205, 90, 106)
#define HEIGHT_WIDTH_RATE 2.6
#define MIN_AREA 200
#define PI 3.14159
#define SIZELENGTH 220
#define DELTA_ANGLE 10
#define DELTA_COORD 10
#define ARMOR_NUM 100
#define REAL_ARMOR_LENGTH 120
#define RED 0
#define BLUE 1

using namespace std;
using namespace cv;

/*函数名称：kbhit
 *参数：void
 *函数功能：检测是否有键被按下
 *返回值：若有键被按下，返回1；否则返回0
 */
int kbhit()
{
	struct termios oldt, newt;
	int ch, oldf;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);
	if (ch != EOF)
	{
		ungetc(ch, stdin);
		return 1;
	}
	return 0;
}

struct Armor
{
	Mat image;                              //当前图片的矩阵信息                      
	Point2f corners[ARMOR_NUM][4];          //二维数组，存放每个检测到的装甲板的角点信息
	Point2f center[ARMOR_NUM];              //存放每个检测到的装甲板的中心点信息
	double angles[ARMOR_NUM];               //存放每个检测到的装甲板的角度信息
	double sizelength[ARMOR_NUM] = { 0 };       //存放每个检测到的装甲板的边长信息
	int armor_num = 0;                        //存放装甲板的个数
	Mat armor[ARMOR_NUM];                   //存放截取的装甲板图片的矩阵信息
};

class timer
{
public:
	using clk_t = std::chrono::high_resolution_clock;
	timer() : t(clk_t::now()) {}
	void reset() { t = clk_t::now(); }

	double milli_cnt() const
	{
		return std::chrono::duration<double, std::milli>(clk_t::now() - t).count();
	}

private:
	clk_t::time_point t;
}; // class timer

/*函数名称：Draw_Contours_MinRect
 *参数：Mat binary：传入的图像的矩阵信息
		double* angles：传入的角度数组
		double* centers_x：传入的中心点的x坐标数组
		double* centers_y：传入的中心点的y坐标数组
		int* array_length：传入的angles数组长度
 *函数功能：在二值化的图像上画灯条轮廓和最小矩形
 *返回值：修改后的binary
 */
Mat Draw_Contours_MinRect(Mat binary, double* angles, double* centers_x, double* centers_y, int* array_length)
{
	vector<vector<Point>>contours;
	vector<Vec4i> hierarchy;
	Mat contour_image = binary.clone();
	RotatedRect r_rect;
	double height = 0, width = 0, n = 0, height_width_rate = 0, area = 0;

	findContours(binary, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point());

	vector<RotatedRect> minRect(contours.size());

	for (int i = 0; i < contours.size(); i++)
	{
		r_rect = minAreaRect(contours[i]);
		height = (r_rect.size.height > r_rect.size.width) ? r_rect.size.height : r_rect.size.width;
		width = (r_rect.size.height > r_rect.size.width) ? r_rect.size.width : r_rect.size.height;
		area = width * height;
		height_width_rate = height / width;

		drawContours(contour_image, contours, i, Scalar(255, 255, 255), CV_FILLED);
		minRect[i] = minAreaRect(Mat(contours[i]));

		Point2f rect_points[4];
		minRect[i].points(rect_points);

		//根据矩形长宽比和最低面积阈值筛选矩形
		if (height_width_rate >= HEIGHT_WIDTH_RATE && area >= MIN_AREA)
		{
			for (int j = 0; j < 4; j++)
			{
				line(contour_image, rect_points[j], rect_points[(j + 1) % 4], DRAWER_COLOR, 4, 8, 0);
			}
			angles[*array_length] = (r_rect.size.height > r_rect.size.width) ? (90 + r_rect.angle) : (r_rect.angle);
			centers_x[*array_length] = r_rect.center.x;
			centers_y[*array_length] = r_rect.center.y;
			*array_length += 1;
		}
	}

	int min_x_id = 0, t_x = 0;

	for (int i = 0; i < *array_length; i++)
	{
		for (int j = i + 1; j < *array_length; j++)
		{
			if (centers_x[j] < centers_x[min_x_id])
			{
				t_x = j;
				j = min_x_id;
				min_x_id = t_x;
			}
		}
	}

	return contour_image;
}

/*函数名称：MarchBars_FrameOutArmor
 *参数：Armor contour_image：传入的Armor结构体信息
		int array_length：传入的angles数组长度
		double* angles：传入的angles数组
		double* centers_x：传入的中心点的x坐标数组
		double* centers_y：传入的中心点的y坐标数组
 *函数功能：匹配灯条以及将匹配灯条间的装甲板信息储存
 *返回值：修改后的contour_image
 */
struct Armor MarchBars_FrameOutArmor(Armor contour_image, int array_length, double* angles, double* centers_x, double* centers_y)
{
	float distance = 0;
	float armor_length = 0, armor_width = 0;
	Point2f armor_center;
	RotatedRect armor;
	int n = 0;
	for (int m = 0; m < array_length && centers_x[m]; m++)
	{
		std::cout << 'm' << m << std::endl;
		std::cout << "angle" << angles[m] << std::endl;
		n = m + 1;
		distance = sqrt((centers_x[m] - centers_x[n]) * (centers_x[m] - centers_x[n]) + (centers_y[m] - centers_y[n]) * (centers_y[m] - centers_y[n]));
		if (centers_x[n] && angles[m] <= angles[n] + DELTA_ANGLE && angles[m] >= angles[n] - DELTA_ANGLE && distance >= 200 && distance <= 800)
		{
			armor_center.x = (centers_x[m] + centers_x[n]) / 2;
			armor_center.y = (centers_y[m] + centers_y[n]) / 2;
			armor_length = distance;
			armor_width = armor_length;
			armor = cv::RotatedRect(armor_center, Point2f(armor_length, armor_width), angles[m]);
			contour_image.sizelength[contour_image.armor_num] = distance;
			contour_image.center[contour_image.armor_num].x = armor_center.x;
			contour_image.center[contour_image.armor_num].y = armor_center.y;
			contour_image.angles[contour_image.armor_num] = angles[m];
			armor.points(contour_image.corners[contour_image.armor_num]);
			contour_image.dis_real_rate[contour_image.armor_num] = distance / REAL_ARMOR_LENGTH;
			contour_image.armor_num += 1;
		}
	}

	return contour_image;
}

/*函数名称：Get_Armor
 *参数：Armor armor：传入的Armor结构体信息
 *函数功能：截取出原图中的匹配灯条间的装甲板图片
 *返回值：修改后的armor
 */
struct Armor Get_Armor(Armor armor)
{
	//namedWindow("armor",CV_WINDOW_AUTOSIZE);
	Size ssize;
	Point2f dstpoints[4];
	dstpoints[0] = Point2f(0, 0);
	dstpoints[1] = Point2f(armor.image.cols, 0);
	dstpoints[2] = Point2f(armor.image.cols, armor.image.rows);
	dstpoints[3] = Point2f(0, armor.image.rows);
	Point2f t_corner;
	for (int i = 0; i < armor.armor_num; i++)
	{
		if (armor.angles[i] < 0)
		{
			t_corner = armor.corners[i][0];
			armor.corners[i][0] = armor.corners[i][2];
			armor.corners[i][2] = t_corner;

			t_corner = armor.corners[i][1];
			armor.corners[i][1] = armor.corners[i][3];
			armor.corners[i][3] = t_corner;
		}
		Mat transmat = getPerspectiveTransform(armor.corners[i], dstpoints);
		ssize = transmat.size();
		if (!ssize.empty())
		{
			warpPerspective(armor.image, armor.armor[i], transmat, armor.image.size());
			ssize = armor.armor[i].size();
			if (!ssize.empty())
				cvtColor(armor.armor[i], armor.armor[i], cv::COLOR_BGR2GRAY);

		}
	}

	return armor;
}

/*函数名称：Classify_Pnp
 *参数：Armor& armor
 *函数功能：实现分类器和pnp功能，并框出正确的装甲板，显示pnp信息
 *返回值：无
 */
void Classify_Pnp(Armor& armor)
{
	sp::classifier classifier("./armor");

	int goods_right = 0;
	int bads_right = 0;
	for (int i = 0; i < armor.armor_num; i++)
	{
		Size ssize = armor.armor[i].size();
		if (!ssize.empty())
		{
			goods_right = classifier.boolean_forward(armor.armor[i]);
			bads_right = !classifier.boolean_forward(armor.armor[i]);
		}
		std::cout << goods_right << ' ' << bads_right << std::endl;

		if (goods_right > bads_right)
		{
			for (int j = 0; j < 4; j++)
				line(armor.image, armor.corners[i][j], armor.corners[i][(j + 1) % 4], cv::Scalar(205, 90, 106), 4, 8, 0);
			//pnp
			Mat rvec(3, 1, CV_64FC1), tvec(3, 1, CV_64FC1), camera_matrix(3, 3, CV_64FC1), dist_coeffs(1, 5, CV_64FC1);
			FileStorage fs2("./camera.yml", FileStorage::READ);
			fs2["camera_matrix"] >> camera_matrix;
			fs2["distortion_coefficients"] >> dist_coeffs;
			float pitch, yaw, dist;
			vector<Point3f> obj = vector<Point3f>{
				cv::Point3f(-REAL_ARMOR_LENGTH / 2,-REAL_ARMOR_LENGTH / 2,0),
				cv::Point3f(REAL_ARMOR_LENGTH / 2,-REAL_ARMOR_LENGTH / 2,0),
				cv::Point3f(REAL_ARMOR_LENGTH / 2,REAL_ARMOR_LENGTH / 2,0),
				cv::Point3f(-REAL_ARMOR_LENGTH / 2,REAL_ARMOR_LENGTH / 2,0)
			};
			vector<Point2f> image_points;
			image_points.push_back(armor.corners[i][0]);
			image_points.push_back(armor.corners[i][1]);
			image_points.push_back(armor.corners[i][2]);
			image_points.push_back(armor.corners[i][3]);
			std::cout << armor.corners[i][0].x << ' ' << armor.corners[i][1].x << ' ' << armor.corners[i][2].x << ' ' << armor.corners[i][3].x << endl;
			solvePnP(obj, image_points, camera_matrix, dist_coeffs, rvec, tvec, false, SOLVEPNP_ITERATIVE);

			uchar* ptrbegin = tvec.data;
			const uchar* ptrend = tvec.data + 3;
			float x, y, z;
			x = *(ptrend - 3);
			y = *(ptrend - 2);
			z = *(ptrend - 1);
			pitch = atan(y / z);
			yaw = atan(x / z);
			pitch = 180 * pitch / PI;
			yaw = 180 * yaw / PI;
			dist = sqrt(x * x + y * y + z * z) / 10;
			dist = z / 10;
			std::cout << pitch << "  " << yaw << "  " << dist << endl;
			putText(armor.image, "pitch:" + to_string(pitch), armor.center[i] + Point2f(0, -25),
				FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0));
			putText(armor.image, "yaw: " + to_string(yaw), armor.center[i],
				FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0));
			putText(armor.image, "dist: " + to_string(dist), armor.center[i] + Point2f(0, 25),
				FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0));
		}

	}

}

int main()
{
	VideoCapture capture(1);
	//parameter can be adjusted
	if (!capture.isOpened())
	{
		std::cout << "Unable to open camera." << endl;
	}

	//调整摄像头参数
	capture.set(CAP_PROP_FOURCC, 'GPJM');		
	capture.set(CAP_PROP_EXPOSURE, 5000);		
	capture.set(CAP_PROP_FRAME_WIDTH, 1280);
	capture.set(CAP_PROP_FRAME_HEIGHT, 720);

	Point2f centers[20];
	double angles[ARMOR_NUM] = { 0 }, centers_x[ARMOR_NUM] = { 0 }, centers_y[ARMOR_NUM] = { 0 };
	Mat src, hsv, binary;
	int i = 0, j = 0, k = 0;
	int array_length = 0;
	struct Armor armor, frame, contour_image;
	Size ssize;
	namedWindow("output", CV_WINDOW_AUTOSIZE);
	int flag = RED;

	while (true)
	{
		capture >> frame.image;
		Mat camera_matrix, dist_coeffs;
		FileStorage fs2("./camera.yml", FileStorage::READ);
		fs2["camera_matrix"] >> camera_matrix;
		fs2["distortion_coefficients"] >> dist_coeffs;
		timer t;

		ssize = frame.image.size();
		if (!ssize.empty())
			src = frame.image.clone();
		//RGB To HSV
		cvtColor(src, hsv, COLOR_RGB2HSV);

		//二值化
		//红蓝检测的转换：若有键被按下，则从红到蓝，蓝到红暂时无法实现
		cv::Scalar scalarL, scalarH;
		if (flag == RED)
		{
			scalarL = SCALAR_LOW_RED;
			scalarH = SCALAR_HIGH_RED;

		}
		else
		{
			scalarL = SCALAR_LOW_BLUE;
			scalarH = SCALAR_HIGH_BLUE;
		}
		if (kbhit())
		{
			flag = BLUE;
			scalarL = SCALAR_LOW_BLUE;
			scalarH = SCALAR_HIGH_BLUE;
		}
		inRange(hsv, scalarL, scalarH, binary);

		double t1 = t.milli_cnt();
		std::cout << "binaryzation:" << t1 << endl;

		//在二值化的图像上画灯条轮廓和最小矩形
		array_length = 0;
		ssize = binary.size();
		if (!ssize.empty())
			contour_image.image = Draw_Contours_MinRect(binary, angles, centers_x, centers_y, &array_length);
		std::cout << "Average bench time_contour: " << t.milli_cnt() << " ms\n" << endl << endl;

		for (i = 0; i < array_length; i++)
		{
			centers[i].x = centers_x[i];
			centers[i].y = centers_y[i];
		}
		double t2 = t.milli_cnt();
		std::cout << "contours:" << (t2 - t1) << endl;

		//匹配灯条以及将匹配灯条间的装甲板信息储存
		ssize = frame.image.size();
		if (!ssize.empty())
			contour_image = MarchBars_FrameOutArmor(frame, array_length, angles, centers_x, centers_y);
		std::cout << "Average bench time_frameout: " << t.milli_cnt() << " ms\n" << endl << endl;
		double t3 = t.milli_cnt();
		std::cout << "frame out armor:" << (t3 - t2) << endl;

		//实现分类器和pnp功能，并框出正确的装甲板，显示pnp信息
		ssize = contour_image.image.size();
		if (!ssize.empty() && contour_image.armor_num)
		{
			contour_image = Get_Armor(contour_image);
			std::cout << "Average bench time_get_armor: " << t.milli_cnt() << " ms\n" << endl << endl;
			ssize = contour_image.image.size();
			if (!ssize.empty())
			{
				std::cout << "classify" << endl;
				Classify_Pnp(contour_image);
				std::cout << "Average bench time_classify: " << t.milli_cnt() << " ms\n" << endl << endl;
			}

		}
		double t4 = t.milli_cnt();
		std::cout << "classifier and pnp:" << (t4 - t3) << endl;


		//显示帧率，灯条数目和正确的装甲板数目
		double fps = 1.0 / t.milli_cnt() * 1000;
		putText(contour_image.image, "FPS:" + to_string(fps), Point(5, 25),
			FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255));
		putText(contour_image.image, "Light count: " + to_string(2 * contour_image.armor_num), Point(5, 50),
			FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255));
		putText(contour_image.image, "Armor count: " + to_string(contour_image.armor_num), Point(5, 75),
			FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255));
		ssize = contour_image.image.size();
		if (!ssize.empty())
			imshow("output", contour_image.image);
		if (waitKey(1) != -1)
		{
			break;
		}
		std::cout << "Average bench time: " << t.milli_cnt() << " ms\n" << endl << endl;
	}

	return 0;
}
