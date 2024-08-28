
#include <opencv2/opencv.hpp>

#include "libcbdetect/config.h"
#include "libcbdetect/plot_corners.h"

namespace cbdetect
{

  void plot_corners(const cv::Mat &img, const std::vector<cv::Point2d> &corners, const char *str)
  {
    cv::Mat img_show;
    if (img.channels() != 3)
    {
#if CV_VERSION_MAJOR >= 4
      cv::cvtColor(img, img_show, cv::COLOR_GRAY2BGR);
#else
      cv::cvtColor(img, img_show, CV_GRAY2BGR);
#endif
    }
    else
    {
      img_show = img.clone();
    }
    for (int i = 0; i < corners.size(); ++i)
    {
      cv::circle(img_show, corners[i], 2, cv::Scalar(0, 0, 255), -1);
    }
    cv::imshow(str, img_show);
    cv::waitKey();
  }

  void plot_corners(const cv::Mat &img, const Corner &corners)
  {
    cv::Mat img_show;
    if (img.channels() != 3)
    {
#if CV_VERSION_MAJOR >= 4
      cv::cvtColor(img, img_show, cv::COLOR_GRAY2BGR);
#else
      cv::cvtColor(img, img_show, CV_GRAY2BGR);
#endif
    }
    else
    {
      img_show = img.clone();
    }
    for (int i = 0; i < corners.p.size(); ++i)
    {
      cv::line(img_show, corners.p[i], corners.p[i] + 20 * corners.v1[i], cv::Scalar(255, 0, 0), 2);
      cv::line(img_show, corners.p[i], corners.p[i] + 20 * corners.v2[i], cv::Scalar(0, 255, 0), 2);
      if (!corners.v3.empty())
      {
        cv::line(img_show, corners.p[i], corners.p[i] + 20 * corners.v3[i], cv::Scalar(0, 0, 255), 2);
      }
      cv::circle(img_show, corners.p[i], 3, cv::Scalar(0, 0, 255), -1);
      cv::putText(img_show, std::to_string(i), cv::Point2i(corners.p[i].x - 12, corners.p[i].y - 6),
                  cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1);
    }
    cv::imshow("corners_img", img_show);
    // cv::imwrite("corners_img.png", img_show);
  }

  void plot_corners(const cv::Mat &img, const Corner &corners, const std::string &windowname)
  {
    cv::Mat img_show;
    if (img.channels() != 3)
    {
#if CV_VERSION_MAJOR >= 4
      cv::cvtColor(img, img_show, cv::COLOR_GRAY2BGR);
#else
      cv::cvtColor(img, img_show, CV_GRAY2BGR);
#endif
    }
    else
    {
      img_show = img.clone();
    }
    for (int i = 0; i < corners.p.size(); ++i)
    {
      cv::line(img_show, corners.p[i], corners.p[i] + 20 * corners.v1[i], cv::Scalar(255, 0, 0), 2);
      cv::line(img_show, corners.p[i], corners.p[i] + 20 * corners.v2[i], cv::Scalar(0, 255, 0), 2);
      if (!corners.v3.empty())
      {
        cv::line(img_show, corners.p[i], corners.p[i] + 20 * corners.v3[i], cv::Scalar(0, 0, 255), 2);
      }
      cv::circle(img_show, corners.p[i], 3, cv::Scalar(0, 0, 255), -1);
      cv::putText(img_show, std::to_string(i), cv::Point2i(corners.p[i].x - 12, corners.p[i].y - 6),
                  cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1);
    }
    cv::imshow(windowname, img_show);
  }

  void plot_corners(CameraPosition pos, const cv::Mat &img, chessboardObject &pts)
  {
    cv::Mat img2;
    if (img.channels() == 3)
      img2 = img.clone();
    else
      cv::cvtColor(img, img2, cv::COLOR_GRAY2RGB);
    auto one_image_pts = pts[pos];

    for (auto iter2 = one_image_pts.begin(), itend2 = one_image_pts.end(); iter2 != itend2; iter2++)
    {
      for (auto iter3 = iter2->second.begin(), itend3 = iter2->second.end(); iter3 != itend3; iter3++)
      {
        int opx = iter3->first.x;
        int opy = iter3->first.y;
        cv::putText(img2, std::to_string(opx) + std::to_string(opy), iter3->second, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1);
      }
    }

    cv::imshow("corners_img2", img2);
    cv::waitKey(5);
  }

} // namespace cbdetect
