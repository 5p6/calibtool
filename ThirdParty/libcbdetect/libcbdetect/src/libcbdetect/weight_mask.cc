
#include <unordered_map>
#include <vector>

#include <opencv2/opencv.hpp>

#include "libcbdetect/config.h"
#include "libcbdetect/weight_mask.h"

namespace cbdetect {

std::unordered_map<int, cv::Mat> weight_mask(const std::vector<int>& radius) {
  std::unordered_map<int, cv::Mat> mask;
  for(const auto& r : radius) {
    mask[r]      = cv::Mat::zeros(r * 2 + 1, r * 2 + 1, CV_64F);
    cv::Mat& mat = mask[r];
    for(int v = 0; v < r * 2 + 1; ++v) {
      for(int u = 0; u < r * 2 + 1; ++u) {
        double dist          = std::sqrt((u - r) * (u - r) + (v - r) * (v - r)) / r;
        dist                 = std::min(std::max(dist, 0.7), 1.3);
        mat.at<double>(v, u) = (1.3 - dist) / 0.6;
      }
    }
  }
  return mask;
};

} // namespace cbdetect