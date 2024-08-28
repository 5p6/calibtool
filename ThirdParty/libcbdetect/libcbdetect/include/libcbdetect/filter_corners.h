
#pragma once
#ifndef LIBCBDETECT_FILTER_CORNERS_H
#define LIBCBDETECT_FILTER_CORNERS_H

#include "config.h"
#include "find_corners.h"
#include <opencv2/opencv.hpp>
#include <vector>

namespace cbdetect {

	enum class CornerDirection
	{
		LEFT = 0,
		RIGHT
	};


LIBCBDETECT_DLL_DECL void filter_corners(const cv::Mat& img, const cv::Mat& img_angle, const cv::Mat& img_weight,
                                         Corner& corner, const Params& params);
LIBCBDETECT_DLL_DECL void filter_black_boxes(CameraPosition camera_position, const cv::Mat& image, chessboardObject& pts, Corner& corner, std::vector<Board>& boards);
}

#endif //LIBCBDETECT_FILTER_CORNERS_H
