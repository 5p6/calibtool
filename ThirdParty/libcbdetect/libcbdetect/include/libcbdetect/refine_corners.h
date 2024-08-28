
#pragma once
#ifndef LIBCBDETECT_REFINE_CORNERS_H
#define LIBCBDETECT_REFINE_CORNERS_H

#include "config.h"
#include "find_corners.h"
#include <opencv2/opencv.hpp>

namespace cbdetect {

LIBCBDETECT_DLL_DECL void refine_corners(const cv::Mat& img_du, const cv::Mat& img_dv, const cv::Mat& img_angle,
                                         const cv::Mat& img_weight, Corner& corners, const Params& params);

}

#endif //LIBCBDETECT_REFINE_CORNERS_H
