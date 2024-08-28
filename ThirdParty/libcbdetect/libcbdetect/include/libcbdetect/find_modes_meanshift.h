
#pragma once
#ifndef LIBCBDETECT_FIND_MODES_MEANSHIFT_H
#define LIBCBDETECT_FIND_MODES_MEANSHIFT_H

#include "config.h"
#include <vector>

namespace cbdetect {

LIBCBDETECT_DLL_DECL std::vector<std::pair<int, double>> find_modes_meanshift(const std::vector<double>& hist,
                                                                              double sigma);

}

#endif //LIBCBDETECT_FIND_MODES_MEANSHIFT_H
