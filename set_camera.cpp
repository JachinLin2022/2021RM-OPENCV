/*作者：王晨燕
 *修改者：
 *文件内容描述：定义了getCameraVals函数，功能是得到当前摄像头的参数
				定义了calibrateCamera函数，功能是修改摄像头的参数
 *文件功能描述：实现了对摄像头参数的调整；
 *与其他文件的联系：
 */
#include <iostream>
#include <opencv2/opencv.hpp>  
#include <linux/videodev2.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

using namespace cv;
using namespace std;

void calibrateCamera();
void getCameraVals();
VideoCapture cam(1);
//variables for cam camera
int brightness_slider = 125;
int contrast_slider = 86;
int gain_slider = 77;
int saturation_slider = 34;
int exposure_slider = 500;
int main()
{

	namedWindow("camera 1", WINDOW_AUTOSIZE);
	Mat frame;
	getCameraVals();
	createTrackbar("Brightness", "camera 1", &brightness_slider, 255);
	createTrackbar("Contrast", "camera 1", &contrast_slider, 255);
	createTrackbar("Gain", "camera 1", &gain_slider, 255);
	createTrackbar("Saturation", "camera 1", &saturation_slider, 255);
	createTrackbar("Exposure", "camera 1", &exposure_slider, 2047);
	while (true)
	{
		calibrateCamera();
		cam >> frame;
		imshow("camera 1", frame);
		if (waitKey(3) >= 0) break;
	}
	return(0);
}

/*函数名称：getCameraVals
 *参数：void
 *函数功能：得到当前摄像头的参数
 *返回值：无
 */
void getCameraVals()
{
	brightness_slider = cam.get(CV_CAP_PROP_BRIGHTNESS);

	contrast_slider = cam.get(CV_CAP_PROP_CONTRAST);
	gain_slider = cam.get(CV_CAP_PROP_GAIN);
	saturation_slider = cam.get(CV_CAP_PROP_SATURATION);
	exposure_slider = cam.get(CV_CAP_PROP_EXPOSURE);
}

/*函数名称：calibrateCamera
 *参数：void
 *函数功能：修改摄像头的参数
 *返回值：无
 */
void calibrateCamera()
{
	cam.set(CV_CAP_PROP_AUTO_EXPOSURE, 0.25);
	cam.set(CV_CAP_PROP_BRIGHTNESS, brightness_slider);
	cam.set(CV_CAP_PROP_CONTRAST, contrast_slider);
	cam.set(CV_CAP_PROP_GAIN, gain_slider);
	cam.set(CV_CAP_PROP_SATURATION, saturation_slider);
	cam.set(CV_CAP_PROP_EXPOSURE, exposure_slider);
}