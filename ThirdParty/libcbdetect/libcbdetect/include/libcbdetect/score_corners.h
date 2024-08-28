
#pragma once
#ifndef LIBCBDETECT_SCORE_CORNERS_H
#define LIBCBDETECT_SCORE_CORNERS_H

#include "config.h"
#include "find_corners.h"
#include <opencv2/opencv.hpp>
#include <vector>

namespace cbdetect {

LIBCBDETECT_DLL_DECL void sorce_corners(const cv::Mat& img, const cv::Mat& img_weight,
                                        Corner& corners, const Params& params);

LIBCBDETECT_DLL_DECL void remove_low_scoring_corners(double tau, Corner& corners, const Params& params);

} // namespace cbdetect

#endif //LIBCBDETECT_SCORE_CORNERS_H
