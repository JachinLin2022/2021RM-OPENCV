#include <opencv2/opencv.hpp>
void iterateMat(const cv::Mat &frame){    
    const std::size_t rowCount =frame.isContinuous() ? 1 :frame.rows; 
    const std::size_t columnCount = frame.cols*frame.rows*frame.channels() / rowCount;
    cv::Mat src;
    src.create(frame.size(),frame.type());
    for (std::size_t i = 0; i < rowCount; i++){
        for (double j = 0; j < columnCount; j++){
            int b = frame.at<cv::Vec3b>(i,j)[0];
            int g = frame.at<cv::Vec3b>(i,j)[1];
            int r = frame.at<cv::Vec3b>(i,j)[2];
            src.at<cv::Vec3b>(i,j)[0] = 255-b;
            src.at<cv::Vec3b>(i,j)[1] = 255-g;
            src.at<cv::Vec3b>(i,j)[2] = 255-r;
        }
    }
    cv::namedWindow("colorreverse",cv::WINDOW_AUTOSIZE);
    cv::imshow("colorreverse",src);
    cv::waitKey(0);
}
int main(){ 
    cv::namedWindow("OpenCVCamera",cv::WINDOW_AUTOSIZE);
    cv::VideoCapture capture(0);
        
    while (true){
        cv::Mat frame;
        capture >> frame;
        cv::imshow("OpenCVCamera",frame);
        //std::cout<<frame<<std::endl;
        iterateMat(frame);
        if (cv::waitKey(30)!= -1){ break; }   
    }
    return 0;
}
