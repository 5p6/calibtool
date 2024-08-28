

#pragma once
#ifndef LIBCBDETECT_BOARD_FROM_CORNRES_H
#define LIBCBDETECT_BOARD_FROM_CORNRES_H

#include "config.h"
#include <opencv2/opencv.hpp>
#include <vector>

namespace cbdetect {

LIBCBDETECT_DLL_DECL void boards_from_corners(const cv::Mat& img, const Corner& corners,
                                              std::vector<Board>& boards, const Params& params);

}

#endif //LIBCBDETECT_BOARD_FROM_CORNRES_H
