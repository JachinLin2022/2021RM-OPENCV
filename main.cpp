/**
 * 大二组 Examination
 * 考核截止日期 : 11.15
 * 特别鸣谢:谭梓煊\曾聪 组
 */

/**
 * 在此添加你的头文件
 * #include<    >
 */
#include <iostream>
#include <cmath>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <cstring>
#include<algorithm>
#include"classifier.hpp"

#define HALF_LENGTH 25
#define WIDTH 600
#define HEIGHT 600
using namespace std;
using namespace cv;

VideoCapture cap ;//= VideoCapture(2);
int exposure = -100, bright = 0, con = 0, sat = 0,hmin=0,hmax=0,smax=0,smin=0,vmin=0,vmax=0;
String cascade_name = "cascade.xml";
CascadeClassifier customFaceDetector;
Mat mask,HSVImage;


float getAngle(const cv::RotatedRect& r) {
  return r.size.width < r.size.height ? r.angle : r.angle + 90;
}
float distance(float a, float b)
{
    return sqrt(pow(a, 2) + pow(b, 2));
}
//回调函数
void setCam(int, void *)
{
    cap.set(CAP_PROP_AUTO_EXPOSURE, 0.25); //
    cap.set(CAP_PROP_EXPOSURE, exposure);  //曝光 50 获取摄像头参数
    cap.set(CAP_PROP_BRIGHTNESS, bright);  //亮度
    cap.set(CAP_PROP_SATURATION, sat);     //饱和度 50
    cap.set(CAP_PROP_CONTRAST, con);       //对比度 4
}
//滑动条调节摄像头参数
void setCamParms(VideoCapture cap)
{
    cv::createTrackbar("exposure : ", "cap", &exposure, 100, setCam);
    cv::createTrackbar("bright : ", "cap", &bright, 100, setCam);
    cv::createTrackbar("contrast : ", "cap", &con, 100, setCam);
    cv::createTrackbar("saturation: ", "cap", &sat, 100, setCam);
    setCam(0, 0);
    return;
}
//回调函数
void callBack(int, void*)
{
	inRange(HSVImage, Scalar(hmin, smin, vmin), Scalar(hmax, smax, vmax), mask);
	//输出图像
	imshow("mask", mask);
}
//滑动条调节HSV范围
void setHSV(Mat frame){
    namedWindow("mask",WINDOW_AUTOSIZE);
	cvtColor(frame, HSVImage, COLOR_BGR2HSV);
	createTrackbar("hmin", "mask", &hmin, 255, callBack);
	createTrackbar("hmax", "mask", &hmax, 255, callBack);

	createTrackbar("smin", "mask", &smin, 255, callBack);
	createTrackbar("smax","mask", &smax, 255, callBack);

	createTrackbar("vmin", "mask", &vmin, 255, callBack);
	createTrackbar("vmax", "mask", &vmax, 255, callBack);
	callBack(0, 0);            
}
//读取相机参数
void readCamParams(Mat &cameraMatrix, Mat &distCoeffs)
{
    FileStorage fs2("camera.yml", FileStorage::READ);
    fs2["camera_matrix"] >> cameraMatrix;
    fs2["distortion_coefficients"] >> distCoeffs;
}
//提取旋转矩形内图像
Mat Image_rRect(RotatedRect a, Mat srcimage)
{
    float _angle = 0.0;
    Size _size = a.size;
    if (a.size.width <= a.size.height)
    {
        //if image is up side down
        //_angle = a.angle - 180;
        _angle = a.angle;
        int temp = _size.width;
        _size.width = _size.height;
        _size.height = temp;
    }
    else
    {
        _angle = a.angle;
    }

    Mat rotatedmat = getRotationMatrix2D(a.center, _angle, 1); 
    Mat image_to_deal;
    warpAffine(srcimage, image_to_deal, rotatedmat, srcimage.size(), CV_INTER_CUBIC);

    Mat rectImage;
    if((a.center.x+_size.width/2)>image_to_deal.cols){
        _size.width=(image_to_deal.cols-a.center.x-1)*2;
    }
     if((a.center.y+_size.height/2)>image_to_deal.rows){
        _size.height=(image_to_deal.rows-a.center.y-1)*2;
    }
    getRectSubPix(image_to_deal, _size, a.center, rectImage);
    Mat grayImage;
     cvtColor(rectImage, grayImage, COLOR_BGR2GRAY);  
    return grayImage;
}
//分类器
bool classify(RotatedRect armors, Mat frame)
{
    Mat gray;
    gray = Image_rRect(armors, frame);
    if(!gray.empty()){
        sp::classifier classifier("armor");
        if(classifier.boolean_forward(gray)){
            cout<<endl;
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
    
}
bool cmp( Point2f a,Point2f b)
{
    return a.y>b.y;
}
//边界处理
void boundaryTreat(Point2f* points){
    for(int i=0;i<4;i++){
        if(points[i].x<0){
            points[i].x=0;
        }
        if(points[i].x>WIDTH){
            points[i].x=WIDTH;
        }
            if(points[i].y<0){
            points[i].y=0;
        }
        if(points[i].y>HEIGHT){
            points[i].y=HEIGHT;
        }

    }

}
int main()
{
    namedWindow("cap", 2);
    resizeWindow("cap",WIDTH,HEIGHT);
	VideoCapture cap = VideoCapture(2);
    if (!cap.isOpened())
    {
        cout << "open camera failed" << endl;
        return -1;
    }

    /*
    * 在此添加相机参数调整部分(如曝光\对比度等)
    * bool setCamParms(.....)
    * bonus: 使用更加人性化的调整方式(如滑动条)
    */
    // setCamParms(cap);
    cap.set(CAP_PROP_AUTO_EXPOSURE, 0.25); //
    cap.set(CAP_PROP_EXPOSURE,0);         //曝光
    cap.set(CAP_PROP_BRIGHTNESS, 0);       //亮度
    cap.set(CAP_PROP_SATURATION, 0);       //饱和度
    cap.set(CAP_PROP_CONTRAST, 0);         //对比度
    cap.set(CAP_PROP_FPS, 30);             //帧率30*/

    /**
 * 在此读入相机矩阵和畸变参数
 * bool readCamParams(Mat& cameraMatrix, Mat& distCoeffs)
 * tips : 建议直接从标定产生的yml文件中读入
 */
    Mat cameraMatrix, distCoeffs;
    readCamParams(cameraMatrix, distCoeffs);

    /**
 * 开始检测
*/
    Mat frame;
    while (true)
    {
        // getTickcount函数：返回从操作系统启动到当前所经过的毫秒数
        // getTickFrequency函数：返回每秒的计时周期数
        double t = getTickCount();
        cout << "===========new frame===========" << endl;
        // 读取一帧
        cap >> frame;
        if(frame.empty()){
            break;
        }
        // 检查键盘输入
        int key = waitKey(1);
        if (key == 'q')
        {
            break;
        }
        else if (key == 't')
        {
            imwrite(to_string(int((long long)t % INT_MAX)) + ".jpg", frame);
        }

        // 进行装甲板检测
        /**
     * 建议设计你们自己的类class Light / class Armor
     * vector<vector<Light>> lightRect;
     * vector<vector<Armor>> armors;
     * 
     * tips : 可以选择继承opencv现有的类
    */

        /*
        * 进行你的图像预处理部分
        * bool prepocess(const Mat& origin, const int& color,  Mat& result....);
        * tips : 请做好灯条颜色的分类
        */

        //setHSV(frame);//调参函数
    Mat HSVImage;
    cvtColor(frame, HSVImage, COLOR_BGR2HSV);
   //inRange(HSVImage, Scalar(30, 30, 105), Scalar(255, 255, 255), mask); //红
   
    inRange(HSVImage, Scalar(97, 74, 81), Scalar(255, 255, 255), mask); //蓝
  
        double t1 = getTickCount();
        cout << " preprocess : " << (t1 - t) / getTickFrequency() << endl;

        /*
        * 筛选灯条
    */
        vector<vector<Point>> lightContours; //灯条轮廓
        //找轮廓
        findContours(mask.clone(), lightContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
        //灯条拟合矩形
        vector<RotatedRect> lightRect;

        for (const auto &contour : lightContours)
        {
            float lightContourArea = contourArea(contour);
            //旋转矩形
            RotatedRect Rect = minAreaRect(contour);
            //根据宽高比排除干扰
            double height = max(Rect.size.width, Rect.size.height);
            double width = min(Rect.size.width, Rect.size.height);
             if (height / width > 10||height/width<2)
            {
                continue;
            }
            //根据面积排除干扰
            if(height*width<500){
                continue;
            }
            lightRect.push_back(Rect);
        }

        double t2 = getTickCount();
        cout << " classify : " << (t2 - t1) / getTickFrequency() << endl;

        /*
        * 进行灯条匹配,并获取装甲板
    */
        vector<vector<RotatedRect>> lights(2000);
        vector<vector<RotatedRect>> armors(2000);
        float dis;
        int k = 0;
        //遍历所有灯条进行匹配
        for (size_t i = 0; i < lightRect.size(); i++)
        {
            for (size_t j = i + 1; (j < lightRect.size()); j++)
            {
                //根据平行
                   if(abs(getAngle(lightRect[i])-getAngle(lightRect[j]))>30){
                    continue;
                }
                //根据灯条距离
                dis = distance((lightRect[i].center.x - lightRect[j].center.x), (lightRect[i].center.y - lightRect[j].center.y));
                float m1 = max(lightRect[i].size.height, lightRect[i].size.width);
                float m2 = max(lightRect[j].size.height, lightRect[j].size.width);
                float ma = max(m1, m2);
                float mi = min(m1, m2);
                //根据板间距离
                if (dis > 3 * ma||dis<ma )
                {
                    continue;
                }
                //根据面积比
                float s1 = lightRect[i].size.height * lightRect[i].size.width;
                float s2 = lightRect[j].size.height * lightRect[j].size.width;
                float sm=max(s1,s2);
                float smi=min(s1,s2);
                if (sm/smi>2)
                {
                   continue;
                }
                if(abs(lightRect[i].center.y-lightRect[j].center.y)>abs(lightRect[i].center.x-lightRect[j].center.x)){
                    continue;
                }
                 
                //灯条
                lights[k].push_back(lightRect[i]);
                lights[k].push_back(lightRect[j]);
                //装甲板
                float angle = min(lightRect[i].angle, lightRect[j].angle);
                Point2f s;
                s.x = (lightRect[i].center.x + lightRect[j].center.x) / 2;
                s.y = (lightRect[i].center.y + lightRect[j].center.y) / 2;
                RotatedRect a = RotatedRect(Point2f(s.x, s.y), Size2f(2 * ma, 2 * ma), angle);
                armors[k].push_back(a);
                k++;
            }
        }
        
        double t3 = getTickCount();
        cout << " findArmor : " << (t3 - t2) / getTickFrequency() << endl;
         
        // 使用你的分类器,获取最终的装甲板
      /*  vector<vector<RotatedRect>> armors_true(20);
         vector<vector<RotatedRect>> ::iterator it = lights.begin();
        for(int i=0;i<k;i++){
        for(auto &&armor : armors[i]){
            if(classify(armor,  frame)){
                armors_true[i].push_back(armor);
            }
            else{
                lights.erase(it+i);
            }
        }
        }
    // * bonus:高效很重要 :-)*/

        double t4 = getTickCount();
        cout << " classify : " << (t4 - t3) / getTickFrequency() << endl;

        //画出你的灯条和装甲板
        for (int i = 0; i < 2; i++)
        {
            for (auto &&light : lights[i])
            {
                
                cv::Point2f *vertices = new cv::Point2f[4];
                light.points(vertices);
                boundaryTreat(vertices);
                for (size_t i = 0; i < 4; i++)
                {
                    cv::line(frame, vertices[i], vertices[(i + 1) % 4], Scalar(255, 255 * (1 - i), 255 * i), 4, 8, 0);
                }
                delete vertices;
            }
            for (auto &&armor : armors[i])
            {
                cv::Point2f *vertices = new cv::Point2f[4];
                armor.points(vertices);
                boundaryTreat(vertices);
                for (size_t i = 0; i < 4; i++)
                {
                    cv::line(frame, vertices[i], vertices[(i + 1) % 4], Scalar(255, 255 * (1 - i), 255 * i), 4, 8, 0);
                }
                double pitch, yaw, dist;
                /*
                * 进行你的距离和角度测量
                * bool solvePosition(...)
                * tips : 好好研究solvepnp的API :-*/
                //自定义的物体世界坐标，单位为mm
                vector<Point3f> obj = vector<Point3f>{
                    cv::Point3f(-HALF_LENGTH, -HALF_LENGTH, 0), 
                    cv::Point3f(HALF_LENGTH, -HALF_LENGTH, 0),  
                    cv::Point3f(HALF_LENGTH, HALF_LENGTH, 0), 
                    cv::Point3f(-HALF_LENGTH, HALF_LENGTH, 0)   
                };
                vector<Point2f> pnts;
                Point2f temp;
                sort(vertices,vertices+4,cmp);
                if(vertices[0].x>vertices[1].x){
                    temp=vertices[0];
                    vertices[0]=vertices[1];
                    vertices[1]=temp;
                }
                 if(vertices[2].x<vertices[3].x){
                    temp=vertices[2];
                    vertices[2]=vertices[3];
                    vertices[3]=temp;
                }
                for (int i = 0; i < 4; i++)
                {
                    pnts.push_back(vertices[i]);
                }

                cv::Mat rVec = cv::Mat::zeros(3, 1, CV_64FC1); //init rvec
                cv::Mat tVec = cv::Mat::zeros(3, 1, CV_64FC1); //init tvec

                solvePnP(obj, pnts, cameraMatrix, distCoeffs, rVec, tVec, false, SOLVEPNP_ITERATIVE);
                pitch =atan2(rVec.at<double>(3, 2), rVec.at<double>(2, 3))/CV_PI*180;
                yaw = atan2(-1*rVec.at<double>(3, 1), sqrt(rVec.at<double>(3, 2)*rVec.at<double>(3, 3)+rVec.at<double>(3, 2)*rVec.at<double>(3, 3)))/CV_PI*180 ;
                dist = sqrt(tVec.at<double>(0, 0) * tVec.at<double>(0, 0) + tVec.at<double>(1, 0) * tVec.at<double>(1, 0) +tVec.at<double>(2, 0) * tVec.at<double>(2, 0));
               
                auto center = armor.center;
                putText(frame, "pitch:" + to_string(pitch), center + Point2f(0, -25),
                        FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0));
                putText(frame, "yaw: " + to_string(yaw), center,
                        FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0));
                putText(frame, "dist: " + to_string(dist), center + Point2f(0, 25),
                        FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0));

                delete vertices;
            }
        }

        double t5 = getTickCount();
        cout << " classify : " << (t5 - t4) / getTickFrequency() << endl;
        // t为该处代码执行所耗的时间,单位为秒,fps为其倒数
        t = (getTickCount() - t) / getTickFrequency();
        float fps = 1.0 / t;
        // 显示FPS
        putText(frame, "FPS:" + to_string(fps), Point(5, 25),
                FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255));
        putText(frame, "Light count: " + to_string(lights[0].size() + lights[1].size()), Point(5, 50),
                FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255));
        putText(frame, "Armor count: " + to_string(armors[0].size() + armors[1].size()), Point(5, 75),
                FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255));
        imshow("cap", frame);
    }

    cap.release();

    destroyAllWindows();

    return 0;
}
