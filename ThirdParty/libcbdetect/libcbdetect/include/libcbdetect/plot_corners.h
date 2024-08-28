
#pragma once
#ifndef LIBCBDETECT_PLOT_CORNERS_H
#define LIBCBDETECT_PLOT_CORNERS_H

#include "config.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>



namespace cbdetect {

LIBCBDETECT_DLL_DECL void plot_corners(const cv::Mat& img, const std::vector<cv::Point2d>& corners, const char* str);

LIBCBDETECT_DLL_DECL void plot_corners(const cv::Mat& img, const Corner& corners);

LIBCBDETECT_DLL_DECL void plot_corners(const cv::Mat& img, const Corner& corners,const std::string& windowname);


//LIBCBDETECT_DLL_DECL void plot_corners(const cv::Mat& img, chessboardObject& pts);

LIBCBDETECT_DLL_DECL void plot_corners(CameraPosition pos, const cv::Mat& img, chessboardObject& pts);

} // namespace cbdetect

#endif //LIBCBDETECT_PLOT_CORNERS_H
