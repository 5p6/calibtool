#include "calib.h"
#include <opencv2/opencv.hpp>
#include "utility.h"
#include <memory.h>
#include <iostream>
#include <string>

void Mono_function(int argc, char *argv[])
{
    if (argc < 6)
    {
        std::cout << "calibrate function : please check the parameters" << std::endl;
        return;
    }
    calibration::Calib::CameraSensorType SensorType = std::string(argv[6]) == "Pinhole" ? calibration::Calib::CameraSensorType::Pinhole : calibration::Calib::CameraSensorType::Fisheye;
    calibration::Calib::ChessboardType ChessboardType = std::string(argv[7]) == "Corner" ? calibration::Calib::ChessboardType::CornerGrid : calibration::Calib::ChessboardType::CircleGrid;
    float squre_size = std::atof(argv[5]);
    cv::Size board_size = cv::Size(std::atoi(argv[3]), std::atoi(argv[4]));
    cv::Size radius_size = cv::Size(5, 5);

    std::shared_ptr<calibration::Calib> calib = calibration::Calib::create(
        SensorType,
        ChessboardType,
        argv[2],
        board_size,
        squre_size,
        radius_size);
    calib->run();
    if (argc == 9)
    {
        calib->write(argv[8]);
    }
}

void Stereo_function(int argc, char *argv[])
{
    if (argc < 6)
    {
        std::cout << "calibrate function : please check the parameters" << std::endl;
        return;
    }
    calibration::Calib::CameraSensorType SensorType = std::string(argv[6]) == "Pinhole" ? calibration::Calib::CameraSensorType::Pinhole : calibration::Calib::CameraSensorType::Fisheye;
    calibration::Calib::ChessboardType ChessboardType = std::string(argv[7]) == "Corner" ? calibration::Calib::ChessboardType::CornerGrid : calibration::Calib::ChessboardType::CircleGrid;
    float squre_size = std::atof(argv[5]);
    cv::Size board_size = cv::Size(std::atoi(argv[3]), std::atoi(argv[4]));
    cv::Size radius_size = cv::Size(5, 5);

    std::shared_ptr<calibration::Calib> calib = calibration::Calib::create(
        SensorType,
        ChessboardType,
        std::string(argv[2]) + "/left",
        std::string(argv[2]) + "/right",
        board_size,
        squre_size,
        radius_size);
    calib->run();
    if (argc == 9)
    {
        calib->write(argv[8]);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 1)
    {
        std::cout << "the Usage \n"
                  << "First : [Exec] Mono [Root_dir] [Board_width] [Board_height] [Square_Size] [Camera_Type] [Board_Type] [Param_File]"
                  << "Second : [Exec] Stereo [Root_dir] [Board_width] [Board_height] [Square_Size] [Camera_Type] [Board_Type] [Param_File]"
                  << std::endl;
    }
    std::cout << argv[1] << std::endl;
    if (std::string(argv[1]) == "Mono")
    {
        Mono_function(argc, argv);
    }
    else if (std::string(argv[1]) == "Stereo")
    {
        Stereo_function(argc, argv);
    }
    else
    {
        std::cout << "the first argv parameter is false ,please check the parameters" << std::endl;
    }
    return 1;
}