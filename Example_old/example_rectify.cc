#include "calib.h"
#include <opencv2/opencv.hpp>
#include "utility.h"


// @brief 利用参数读取构造函数读取参数，然后矫正
void example_rectify(int argc,char* argv[]){
    cv::Mat left,right; 
    cv::Mat rectify_left,rectify_right;
    left = cv::imread(argv[2]);
    cv::imshow("original image",cv::WINDOW_NORMAL);
    cv::imshow("original image",left);

    std::shared_ptr<calibration::Calib> calib = calibration::Calib::create(argv[1]);
    if(argc==4){
        right = cv::imread(argv[3]);
        cv::imshow("original right image",cv::WINDOW_NORMAL);
        cv::imshow("original right image",right);
        // 矫正
        auto [rectify_left,rectify_right] = calib->rectify(left,right);
        // 可伸缩
        cv::namedWindow("right",cv::WINDOW_NORMAL);
        cv::namedWindow("left",cv::WINDOW_NORMAL);
        cv::imshow("right",rectify_right);
        cv::imshow("left",rectify_left);
    } else{
        rectify_left = calib->rectify(left);
        // 可伸缩
        cv::namedWindow("rectify image",cv::WINDOW_NORMAL);
        cv::imshow("rectify image",rectify_left);
    }
    cv::waitKey(0);
    cv::destroyAllWindows();
}


int main(int argc,char* argv[]){
    if(argc < 3){
        std::cout<<"the Usage \n"
                 <<"[Exec] [Param_File] [Left_path] [Right_path](optional)"
                 << std::endl; 
        return 0;
    }
    example_rectify(argc,argv);
    return 1;
}