#include "calib.h"
#include <opencv2/opencv.hpp>
#include "utility.h"
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <string>

void stereo_rectify(const std::string _left_path,const std::string _right_path,const std::string _param_path)
{
    cv::Mat left = cv::imread(_left_path);
    cv::Mat right = cv::imread(_right_path);
    cv::imshow("original left image",left);
    cv::imshow("original right image",right);
    cv::waitKey();
    cv::destroyAllWindows();
    // 参数读取
    std::shared_ptr<calibration::Calib> calib = calibration::Calib::create(_param_path);
    auto [rectify_left,rectify_right] = calib->rectify(left,right);
    // 可伸缩
    cv::namedWindow("right",cv::WINDOW_NORMAL);
    cv::namedWindow("left",cv::WINDOW_NORMAL);
    cv::imshow("right",rectify_right);
    cv::imshow("left",rectify_left);
    cv::waitKey();
    cv::destroyAllWindows();
}

// @brief 利用参数读取构造函数读取参数，然后矫正
void mono_rectify(const std::string _img_path,const std::string _param_path){
    cv::Mat left; 
    cv::Mat rectify_left;
    left = cv::imread(_img_path);
    cv::imshow("original image",cv::WINDOW_NORMAL);
    cv::imshow("original image",left);
    // 读取参数
    std::shared_ptr<calibration::Calib> calib = calibration::Calib::create(_param_path);
    rectify_left = calib->rectify(left);
    // 可伸缩
    cv::namedWindow("rectify image",cv::WINDOW_NORMAL);
    cv::imshow("rectify image",rectify_left);
    cv::waitKey(0);
    cv::destroyAllWindows();
}


int main(int argc,char* argv[]){

    std::string left_path,right_path,param_path;
    boost::program_options::options_description desc("camera rectify");
    desc.add_options()
    ("help", "produce help message")
    ("left_path,l", boost::program_options::value<std::string>(&left_path), "左图像路径(required)")
    ("right_path,r", boost::program_options::value<std::string>(&right_path), "右图像路径")
    ("param_path,p",boost::program_options::value<std::string>(&param_path),"参数文件路径(required)")
    ;

    // 解析命令行参数
    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);

    // 检查是否请求帮助
    if (vm.count("help"))
    {
        std::cout << desc << "\n";
        return 1;
    }
    if (right_path.empty()){
        mono_rectify(left_path,param_path);
        return 1;
    }
    stereo_rectify(left_path,right_path,param_path);
    return 1;
}