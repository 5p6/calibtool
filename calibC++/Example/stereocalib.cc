#include <calib.h>
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

int main(int argc, char *argv[])
{
    std::string left_dir,right_dir,param_dir,param_path;
    std::string sensor_type, chessboard_type;
    cv::Size board_size(0, 0);
    cv::Size radius_size(5, 5);
    float square_size;
    bool verbose;

    boost::program_options::options_description desc("monocular calibration");
    desc.add_options()
    ("help", "produce help message")
    ("left_dir,l", boost::program_options::value<std::string>(&left_dir), "左图像根目录(required)")
    ("right_dir,r", boost::program_options::value<std::string>(&right_dir), "右图像根目录(required)")
    ("sensor-type", boost::program_options::value<std::string>(&sensor_type)->default_value("Pinhole"), "传感器类型(Pinhole或者Fisheye)")
    ("chessboard-type", boost::program_options::value<std::string>(&chessboard_type)->default_value("Corner"), "棋盘格类型(Corner或者Circle)")
    ("height,h", boost::program_options::value<int>(&board_size.height), "棋盘格规格的高(required)")
    ("width,w", boost::program_options::value<int>(&board_size.width), "棋盘格规格的宽(required)")
    ("square,s", boost::program_options::value<float>(&square_size), "棋盘格小格子尺寸(required)")
    ("radius-height", boost::program_options::value<int>(&radius_size.height)->default_value(5), "亚像素角点查找的高度半径")
    ("radius-width", boost::program_options::value<int>(&radius_size.width)->default_value(5), "亚像素角点查找的宽度半径")
    ("outputdir,o",boost::program_options::value<std::string>(&param_dir),"保存目录路径")
    ("verbose,v",boost::program_options::bool_switch(&verbose)->default_value(false),"是否查看具体的内容")
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
    std::cout<<"左图像根目录 : "<<left_dir<<std::endl
             <<"右图像根目录 : "<<right_dir<<std::endl
             <<"相机传感器类别 : "<<sensor_type<<std::endl
             <<"标定板类别 : "<<chessboard_type<<std::endl
             ;

    calibration::Calib::CameraSensorType SensorType = sensor_type == "Pinhole" ? calibration::Calib::CameraSensorType::Pinhole : calibration::Calib::CameraSensorType::Fisheye;
    calibration::Calib::ChessboardType ChessboardType = chessboard_type == "Corner" ? calibration::Calib::ChessboardType::CornerGrid : calibration::Calib::ChessboardType::CircleGrid;
    std::shared_ptr<calibration::Calib> calib = calibration::Calib::create(
        SensorType,
        ChessboardType,
        left_dir,
        right_dir,
        board_size,
        square_size,
        radius_size);
    calib->setflagshow(verbose);
    // 运行
    calib->run();
    if(!param_dir.empty()){
        std::cout<< "保存参数,"<<"保存路径为 : "<<param_dir<<std::endl;
        if(!boost::filesystem::exists(param_dir)){
            boost::filesystem::create_directories(param_dir);
        }
        calib->write(param_dir + "/" + "stereocalib.yaml");
        return 1;
    }
    std::cout<<"此次不保存参数！"<<std::endl;
    return 1;
}