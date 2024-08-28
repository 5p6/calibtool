#ifndef CAMERACONFIG_H
#define CAMERACONFIG_H


#include "config.h" 
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>
class CALIB_API CameraType
{
public:
    enum CameraNumType
    {
        Monocular = 0,
        Stereo
    };

    enum CameraSensorType
    {
        Pinhole = 0,
        Fisheye,
        RGBD
    };

public:
    CameraType() = delete;

    // 初始化
    explicit CameraType(const std::string &param_file);
    explicit CameraType(CameraSensorType _sensorType, CameraNumType _numType,double _SquareSize) : sensortype(_sensorType), numtype(_numType),SquareSize(_SquareSize)
    {}

    // 初始化矫正参数
    void initremap();

    // 读取参数
    void read(const std::string &param_file);
    void write(const std::string &param_file) const;
    // 图像
    cv::Mat rectify(const cv::Mat &img);
    std::pair<cv::Mat, cv::Mat> rectify(const cv::Mat &left, const cv::Mat &right);

    // RGBD 图像配准,to do
    cv::Mat registration(const cv::Mat &depth);

    // 总结
    void Brief();

public:
    // 类型
    CameraNumType numtype;
    CameraSensorType sensortype;
    // 参数矩阵
    cv::Mat K_l, K_r, D_l, D_r;
    cv::Mat R, t;
    cv::Mat R_l, R_r, P_l, P_r, Q;
    // 矫正参数
    cv::Mat maplx, maply, maprx, mapry;
    cv::Size ImageSize;
    // 棋盘格尺寸
    double SquareSize;
};

#endif