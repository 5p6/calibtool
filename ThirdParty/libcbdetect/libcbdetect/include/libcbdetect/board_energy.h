
#pragma once
#ifndef LIBCBDETECT_BOARD_ENERGY_H
#define LIBCBDETECT_BOARD_ENERGY_H

#include "config.h"
#include <vector>

namespace cbdetect {

LIBCBDETECT_DLL_DECL cv::Point3i board_energy(const Corner& corners, Board& board, const Params& params);

}

#endif //LIBCBDETECT_BOARD_ENERGY_H
