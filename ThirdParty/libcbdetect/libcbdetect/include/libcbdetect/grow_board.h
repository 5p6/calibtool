
#pragma once
#ifndef LIBCBDETECT_GROW_BOARD_H
#define LIBCBDETECT_GROW_BOARD_H

#include "config.h"
#include <vector>

namespace cbdetect {

enum GrowType {
  GrowType_Failure = 0,
  GrowType_Inside,
  GrowType_Boundary,
};

LIBCBDETECT_DLL_DECL GrowType grow_board(const Corner& corners, std::vector<int>& used, Board& board,
                                         std::vector<cv::Point2i>& proposal, int direction, const Params& params);

} // namespace cbdetect

#endif //LIBCBDETECT_GROW_BOARD_H
