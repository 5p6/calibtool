
#include <tuple>

#include <opencv2/opencv.hpp>

#include "libcbdetect/board_energy.h"

namespace cbdetect {

cv::Point3i board_energy(const Corner& corners, Board& board, const Params& params) {
  // energy: number of corners
  double E_corners = -1.0 * board.num;

  // energy: structure
  double max_E_structure = std::numeric_limits<double>::min();
  int res_x = 0, res_y = 0, res_z = 0;

  // walk through v1
  for(int i = 0; i < board.idx.size(); ++i) {
    for(int j = 0; j < board.idx[i].size() - 2; ++j) {
      int idx1 = board.idx[i][j];
      int idx2 = board.idx[i][j + 1];
      int idx3 = board.idx[i][j + 2];
      if(idx1 >= 0 && idx2 >= 0 && idx3 >= 0) {
        const cv::Point2d& x1 = corners.p[idx1];
        const cv::Point2d& x2 = corners.p[idx2];
        const cv::Point2d& x3 = corners.p[idx3];
        double E_structure    = cv::norm(x1 + x3 - 2 * x2) / cv::norm(x1 - x3);
        board.energy[i][j][0] = E_corners * (1 - E_structure);
        if(E_structure > max_E_structure) {
          max_E_structure = E_structure;
          res_x           = j;
          res_y           = i;
          res_z           = 0;
        }
      }
    }
  }

  if(params.corner_type == MonkeySaddlePoint) {
    // walk through v2
    for(int i = 0; i < board.idx.size() - 2; ++i) {
      for(int j = 0; j < board.idx[i].size() - 2; ++j) {
        int idx1 = board.idx[i][j];
        int idx2 = board.idx[i + 1][j + 1];
        int idx3 = board.idx[i + 2][j + 2];
        if(idx1 >= 0 && idx2 >= 0 && idx3 >= 0) {
          const cv::Point2d& x1 = corners.p[idx1];
          const cv::Point2d& x2 = corners.p[idx2];
          const cv::Point2d& x3 = corners.p[idx3];
          double E_structure    = cv::norm(x1 + x3 - 2 * x2) / cv::norm(x1 - x3);
          board.energy[i][j][1] = E_corners * (1 - E_structure);
          if(E_structure > max_E_structure) {
            max_E_structure = E_structure;
            res_x           = j;
            res_y           = i;
            res_z           = 1;
          }
        }
      }
    }
  }

  // walk through v3
  for(int i = 0; i < board.idx.size() - 2; ++i) {
    for(int j = 0; j < board.idx[i].size(); ++j) {
      int idx1 = board.idx[i][j];
      int idx2 = board.idx[i + 1][j];
      int idx3 = board.idx[i + 2][j];
      if(idx1 >= 0 && idx2 >= 0 && idx3 >= 0) {
        const cv::Point2d& x1 = corners.p[idx1];
        const cv::Point2d& x2 = corners.p[idx2];
        const cv::Point2d& x3 = corners.p[idx3];
        double E_structure    = cv::norm(x1 + x3 - 2 * x2) / cv::norm(x1 - x3);
        board.energy[i][j][2] = E_corners * (1 - E_structure);
        if(E_structure > max_E_structure) {
          max_E_structure = E_structure;
          res_x           = j;
          res_y           = i;
          res_z           = 2;
        }
      }
    }
  }

  // final energy
  return {res_x, res_y, res_z};
}

} // namespace cbdetect
