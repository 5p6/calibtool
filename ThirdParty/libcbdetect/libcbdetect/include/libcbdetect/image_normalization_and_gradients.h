
#pragma once
#ifndef LIBCBDETECT_BOX_FILTER_H
#define LIBCBDETECT_BOX_FILTER_H

#include "config.h"
#include <opencv2/opencv.hpp>

namespace cbdetect {

LIBCBDETECT_DLL_DECL void box_filter(const cv::Mat& img, cv::Mat& blur_img, int kernel_size_x, int kernel_size_y = -1);

LIBCBDETECT_DLL_DECL void image_normalization_and_gradients(cv::Mat& img, cv::Mat& img_du, cv::Mat& img_dv,
                                                            cv::Mat& img_angle, cv::Mat& img_weight,
                                                            const Params& params);

} // namespace cbdetect

#endif //LIBCBDETECT_BOX_FILTER_H
