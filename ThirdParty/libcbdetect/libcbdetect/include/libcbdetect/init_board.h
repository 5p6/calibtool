
#pragma once
#ifndef LIBCBDETECT_INIT_BOARD_H
#define LIBCBDETECT_INIT_BOARD_H

#include "config.h"
#include <vector>

namespace cbdetect {

LIBCBDETECT_DLL_DECL bool init_board(const Corner& corners, std::vector<int>& used, Board& board, int idx);

}

#endif //LIBCBDETECT_INIT_BOARD_H
