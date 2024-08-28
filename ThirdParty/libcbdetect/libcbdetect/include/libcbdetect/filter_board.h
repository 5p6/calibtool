
#pragma once
#ifndef LIBCBDETECT_FILTER_BOARD_H
#define LIBCBDETECT_FILTER_BOARD_H

#include "config.h"
#include <opencv2/opencv.hpp>
#include <vector>

namespace cbdetect {

LIBCBDETECT_DLL_DECL void filter_board(const Corner& corners, std::vector<int>& used, Board& board,
                                       std::vector<cv::Point2i>& proposal, double& energy, const Params& params);

LIBCBDETECT_DLL_DECL 	void refine_board(CameraPosition camera_postion_label,const Corner& corners, std::vector<Board>& boards, cv::Mat &image);

LIBCBDETECT_DLL_DECL 	void get_board_object(cbdetect::CameraPosition position, const cbdetect::Corner& corners, std::vector<cbdetect::Board>& boards, chessboardObject &ptsWithIndex);
}

#endif //LIBCBDETECT_FILTER_BOARD_H
