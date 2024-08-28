
#pragma once
#ifndef LIBCBDETECT_WEIGHT_MASK
#define LIBCBDETECT_WEIGHT_MASK

#include "config.h"
#include <opencv2/opencv.hpp>
#include <unordered_map>
#include <vector>

namespace cbdetect {

LIBCBDETECT_DLL_DECL std::unordered_map<int, cv::Mat> weight_mask(const std::vector<int>& radius);

}

#endif //LIBCBDETECT_WEIGHT_MASK
