
#pragma once
#ifndef LIBCBDETECT_CREATE_CORRELATION_PATCH_H
#define LIBCBDETECT_CREATE_CORRELATION_PATCH_H

#include "config.h"
#include <opencv2/opencv.hpp>
#include <vector>

namespace cbdetect {

LIBCBDETECT_DLL_DECL void create_correlation_patch(std::vector<cv::Mat>& template_kernel,
                                                   double angle_1, double angle_2, int radius);

LIBCBDETECT_DLL_DECL void create_correlation_patch(std::vector<cv::Mat>& template_kernel,
                                                   double angle_1, double angle_2, double angle_3, int radius);

} // namespace cbdetect

#endif //LIBCBDETECT_CREATE_CORRELATION_PATCH_H
