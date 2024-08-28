
#pragma once
#ifndef LIBCBDETECT_PLOT_DELTILLES_H
#define LIBCBDETECT_PLOT_DELTILLES_H

#include "config.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>
#include <string>

namespace cbdetect {

LIBCBDETECT_DLL_DECL void plot_boards(const cv::Mat& img, const Corner& corners,
                                      const std::vector<Board>& boards, const Params& params);

LIBCBDETECT_DLL_DECL void plot_boards(const cv::Mat& img, const Corner& corners,
                                      const std::vector<Board>& boards, const Params& params,
                                      const std::string& windowname
                                      );

LIBCBDETECT_DLL_DECL void my_plot_boards(CameraPosition pos, const cv::Mat& img, const Corner& corners, const std::vector<Board>& boards, const Params& params, const std::string& path = " ", int num = -1);

LIBCBDETECT_DLL_DECL void getAlignedImageAndPatternPoints(const Corner& corners, const std::vector<Board>& boards, cv::Mat& objectPoints, cv::Mat& imagePoints, double dx = 30, double dy = 30);
}

#endif //LIBCBDETECT_PLOT_DELTILLES_H
