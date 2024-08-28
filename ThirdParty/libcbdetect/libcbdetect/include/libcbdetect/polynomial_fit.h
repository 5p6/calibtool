
#pragma once
#ifndef LIBCBDETECT_POLYNOMIAL_FIT_H
#define LIBCBDETECT_POLYNOMIAL_FIT_H

#include "config.h"
#include <opencv2/opencv.hpp>

namespace cbdetect {

void polynomial_fit(const cv::Mat& img, Corner& corners, const Params& params);

}

#endif //LIBCBDETECT_POLYNOMIAL_FIT_H
