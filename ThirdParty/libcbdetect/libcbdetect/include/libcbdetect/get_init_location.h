
#pragma once
#ifndef LIBCBDETECT_GET_INIT_LOCATION_H
#define LIBCBDETECT_GET_INIT_LOCATION_H

#include "config.h"
#include <opencv2/opencv.hpp>
#include <vector>

namespace cbdetect {

void get_init_location(const cv::Mat& img, const cv::Mat& img_du, const cv::Mat& img_dv,
                       Corner& corners, const Params& parmas);

}

#endif //LIBCBDETECT_GET_INIT_LOCATION_H
