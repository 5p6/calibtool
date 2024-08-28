
#pragma once
#ifndef LIBCBDETECT_NON_MAXIMUM_SUPPRESSION_H
#define LIBCBDETECT_NON_MAXIMUM_SUPPRESSION_H

#include "config.h"
#include "find_corners.h"
#include <opencv2/opencv.hpp>

namespace cbdetect {

LIBCBDETECT_DLL_DECL void non_maximum_suppression(const cv::Mat& img, int n, double tau, int margin, Corner& corners);

LIBCBDETECT_DLL_DECL void non_maximum_suppression_sparse(Corner& corners, int n, cv::Size img_size,
                                                         const Params& params);

} // namespace cbdetect

#endif //LIBCBDETECT_NON_MAXIMUM_SUPPRESSION_H
