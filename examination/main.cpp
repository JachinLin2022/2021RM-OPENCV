/**
 * 大二组 Examination
 * 考核截止日期 : 11.15
 * 特别鸣谢:谭梓煊\曾聪 组
 */

#include <iostream>
#include <opencv2/opencv.hpp>
/**
 * 在此添加你的头文件
 * #include<    >
 */
using namespace std;
using namespace cv;

int main()
{
    VideoCapture cap = VideoCapture(0);
    if (!cap.isOpened()) {
        cout << "open camera failed" << endl;
        return -1;
    }
    /*
     * 在此添加相机参数调整部分(如曝光\对比度等)
     * bool setCamParms(.....)
     * bonus: 使用更加人性化的调整方式(如滑动条)
     */
    Mat cameraMatrix, distCoeffs;
    /**
     * 在此读入相机矩阵和畸变参数
     * bool readCamParams(Mat& cameraMatrix, Mat& distCoeffs)
     * tips : 建议直接从标定产生的yml文件中读入
     */
    
    Mat frame;
    /**
     * 开始你们的检测
     * bonus : 多线程要考虑一下吗 @_@
    */
    while (true) {
        // getTickcount函数：返回从操作系统启动到当前所经过的毫秒数
        // getTickFrequency函数：返回每秒的计时周期数
        double t = getTickCount();
        cout << "===========new frame===========" << endl;
        // 读取一帧
        cap >> frame;

        // 检查键盘输入
        int key = waitKey(1);
        if (key == 'q') {
            break;
        } else if (key == 't') {
            imwrite(to_string(int((long long)t % INT_MAX)) + ".jpg", frame);
        }

        // 进行装甲板检测
        /**
         * 建议设计你们自己的类class Light / class Armor
         * vector<vector<Light>> lights;
         * vector<vector<Armor>> armors;
         * 
         * tips : 可以选择继承opencv现有的类
        */

        /*
         * 进行你的图像预处理部分
         * bool prepocess(const Mat& origin, const int& color,  Mat& result....);
         * tips : 请做好灯条颜色的分类
        */
        double t1 = getTickCount();
        cout << " preprocess : " << (t1- t) / getTickFrequency() <<endl;

        /*
         * 筛选你的灯条
         * bool findLight(....);
        */
        double t2 = getTickCount();
        cout << " findLight : " << (t2- t1) / getTickFrequency() <<endl;

        /*
         * 进行你的灯条匹配,并获取装甲板
         * bool findArmor(....);
        */
        double t3 = getTickCount();
        cout << " findArmor : " << (t3- t2) / getTickFrequency() <<endl;

        /*
         * 使用你的分类器,获取最终的装甲板
         * bool classify(....);
         * bonus:高效很重要 :-)
        */
        double t4 = getTickCount();
        cout << " classify : " << (t4- t3) / getTickFrequency() <<endl;


        for (int i = 0; i < 2; i++) {
            /**画出你的灯条和装甲板*/
            for (auto &&light : lights[i]) {
                light.draw(frame, Scalar(255, 255 * (1 - i), 255 * i), 3);
            }

            for (auto &&armor : armors[i]) {
                double pitch, yaw, dist;
                /**
                 * 进行你的距离和角度测量
                 * bool solvePosition(...)
                 * tips : 好好研究solvepnp的API :-(
                */
               
                auto center = armor.center;
                putText(frame, "pitch:" + to_string(pitch), center+Point2f(0,-25),
                    FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0));
                putText(frame, "yaw: " + to_string(yaw), center,
                    FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0));
                putText(frame, "dist: " + to_string(dist), center+Point2f(0,25),
                    FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0));

                armor.draw(frame, Scalar(255, 255 * (1 - i), 255 * i), 3);
            }
        }

        double t5 = getTickCount();
        cout << " classify : " << (t5- t4) / getTickFrequency() <<endl;


        // t为该处代码执行所耗的时间,单位为秒,fps为其倒数
        t = (getTickCount() - t) / getTickFrequency();
        float fps = 1.0 / t;
        // 显示FPS
        putText(frame, "FPS:" + to_string(fps), Point(5, 25),
            FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255));
        putText(frame, "Light count: " + to_string(lights[0].size()+lights[1].size()), Point(5, 50),
            FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255));
        putText(frame, "Armor count: " + to_string(armors[0].size()+armors[1].size()), Point(5, 75),
            FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255));

        imshow("cap", frame);
    }

    cap.release();

    destroyAllWindows();

    return 0;
}
