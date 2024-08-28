
#include <math.h>
#include <stdio.h>

#include <algorithm>
#include <unordered_map>
#include <vector>
#include <Eigen/Core>
#include <Eigen/LU>
#include <opencv2/opencv.hpp>

#include "libcbdetect/config.h"
#include "libcbdetect/filter_corners.h"
#include "libcbdetect/find_corners.h"
#include "libcbdetect/find_modes_meanshift.h"
#include "libcbdetect/weight_mask.h"
#include "init_board.h"

namespace cbdetect {

void filter_corners(const cv::Mat& img, const cv::Mat& img_angle, const cv::Mat& img_weight,
                    Corner& corners, const Params& params) {
  int n_cicle, n_bin, crossing_thr, need_crossing, need_mode;
  if(params.corner_type == SaddlePoint) {
    n_cicle = n_bin = 32;
	crossing_thr = 3;
	need_crossing = 4;
    need_mode       = 2;
  } else if(params.corner_type == MonkeySaddlePoint) {
    n_cicle       = 48;
    n_bin         = 32;
    crossing_thr  = 3;
    need_crossing = 6;
    need_mode     = 3;
  }
  int width = img.cols, height = img.rows;
  std::vector<cv::Point2d> corners_out_p;
  std::vector<int> corners_out_r;
  std::vector<int> choose(corners.p.size(), 0);
  std::vector<double> cos_v(n_cicle), sin_v(n_cicle);
  for(int i = 0; i < n_cicle; ++i) {
    cos_v[i] = std::cos(i * 2.0 * M_PI / (n_cicle - 1));
    sin_v[i] = std::sin(i * 2.0 * M_PI / (n_cicle - 1));
  }
  auto mask = weight_mask(params.radius);

  cv::parallel_for_(cv::Range(0, corners.p.size()), [&](const cv::Range& range) -> void {
    for(int i = range.start; i < range.end; ++i) {
      int num_crossings = 0, num_modes = 0;
      int center_u = std::round(corners.p[i].x);
      int center_v = std::round(corners.p[i].y);
      int r        = corners.r[i];
      if(center_u - r < 0 || center_u + r >= width - 1 || center_v - r < 0 || center_v + r >= height - 1) {
        continue;
      }

      // extract circle locations and its value
      std::vector<double> c(n_cicle);
      for(int j = 0; j < n_cicle; ++j) {
        int circle_u = static_cast<int>(std::round(center_u + 0.75 * r * cos_v[j]));
        int circle_v = static_cast<int>(std::round(center_v + 0.75 * r * sin_v[j]));
        circle_u     = std::min(std::max(circle_u, 0), width - 1);
        circle_v     = std::min(std::max(circle_v, 0), height - 1);
        c[j]         = img.at<double>(circle_v, circle_u);
      }
      auto minmax  = std::minmax_element(c.begin(), c.end());
      double min_c = *minmax.first, max_c = *minmax.second;
      for(int j = 0; j < n_cicle; ++j) {
        c[j] = c[j] - min_c - (max_c - min_c) / 2;
      }

      // count number of zero-crossings
      int fisrt_cross_index = 0;
      for(int j = 0; j < n_cicle; ++j) {
        if((c[j] > 0) ^ (c[(j + 1) % n_cicle] > 0)) {
          fisrt_cross_index = (j + 1) % n_cicle;
          break;
        }
      }
      for(int j = fisrt_cross_index, count = 1; j < n_cicle + fisrt_cross_index; ++j, ++count) {
        if((c[j % n_cicle] > 0) ^ (c[(j + 1) % n_cicle] > 0)) {
          if(count >= crossing_thr) {
            ++num_crossings;
          }
          count = 1;
        }
      }

      int top_left_u         = std::max(center_u - r, 0);
      int top_left_v         = std::max(center_v - r, 0);
      int bottom_right_u     = std::min(center_u + r, width - 1);
      int bottom_right_v     = std::min(center_v + r, height - 1);
      cv::Mat img_weight_sub = cv::Mat::zeros(2 * r + 1, 2 * r + 1, CV_64F);
      img_weight.rowRange(top_left_v, bottom_right_v + 1).colRange(top_left_u, bottom_right_u + 1).copyTo(img_weight_sub(cv::Range(top_left_v - center_v + r, bottom_right_v - center_v + r + 1), cv::Range(top_left_u - center_u + r, bottom_right_u - center_u + r + 1)));
      img_weight_sub    = img_weight_sub.mul(mask[r]);
      double tmp_maxval = 0;
      cv::minMaxLoc(img_weight_sub, NULL, &tmp_maxval);
      img_weight_sub.forEach<double>([&tmp_maxval](double& val, const int* pos) -> void {
        val = val < 0.5 * tmp_maxval ? 0 : val;
      });

      // create histogram
      std::vector<double> angle_hist(n_bin, 0);
      for(int j2 = top_left_v; j2 <= bottom_right_v; ++j2) {
        for(int i2 = top_left_u; i2 <= bottom_right_u; ++i2) {
          int bin = static_cast<int>(std::floor(img_angle.at<double>(j2, i2) / (M_PI / n_bin))) % n_bin;
          angle_hist[bin] += img_weight_sub.at<double>(j2 - center_v + r, i2 - center_u + r);
        }
      }

      auto modes = find_modes_meanshift(angle_hist, 1.5);
      for(const auto& j : modes) {
        if(2 * j.second > modes[0].second) {
          ++num_modes;
        }
      }

      if(num_crossings == need_crossing && num_modes == need_mode) {
        choose[i] = 1;
      }
    }
  });

  for(int i = 0; i < corners.p.size(); ++i) {
    if(choose[i] == 1) {
      corners_out_p.emplace_back(cv::Point2d(corners.p[i].x, corners.p[i].y));
      corners_out_r.emplace_back(corners.r[i]);
    }
  }
  corners.p = std::move(corners_out_p);
  corners.r = std::move(corners_out_r);
}

Eigen::Vector3f chessboard_curve_fit(std::vector<cv::Point2f> &pts)
	{
		std::vector<float> x_values;
		std::vector<float> y_values;
		//int entry_number;

		for (int i = 0; i < pts.size(); i++)
		{
			x_values.push_back(pts[i].x);
			y_values.push_back(pts[i].y);
		}

		const int entry_number = x_values.size();
		Eigen::MatrixXf mat_u(entry_number, 3);
		Eigen::MatrixXf mat_y(entry_number, 1);

		for (int i = 0; i < entry_number; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				mat_u(i, j) = pow(x_values[i], j);
			}
		}

		for (int i = 0; i < entry_number; ++i)
		{
			mat_y(i, 0) = y_values[i];
		}

		Eigen::MatrixXf K(3, 1);
		Eigen::MatrixXf UUT = mat_u.transpose()*mat_u;
		Eigen::MatrixXf UUTI = UUT.inverse();
		K = UUTI*mat_u.transpose()*mat_y;


		return Eigen::Vector3f(K(0), K(1), K(2));
	}

	void find_pts_on_black_boxes(int left_bounds, int right_bounds, int up_bounds, int down_bounds, int kernel_width, int kernel_height, const cv::Mat &image, Corner &corner, std::vector<cv::Point2f> &black_box_pts)
	{
		cv::Mat image2 = image.clone();
		cv::Mat image3;		
		if (image2.channels() == 3)
		{
			cv::cvtColor(image2, image3, cv::COLOR_RGB2GRAY);
		}
		else
		{
			image3 = image2;
		}
		cv::Mat interest_work_1 = image3.colRange(left_bounds, right_bounds).rowRange(up_bounds, down_bounds).clone();

		float center_y = interest_work_1.rows / 2;
		float center_x = interest_work_1.cols / 2;

		cv::Mat interest_work_2;
		cv::Mat interest;
		auto kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernel_width, kernel_height));
		cv::morphologyEx(interest_work_1, interest_work_2, cv::MORPH_CLOSE, kernel);
		cv::threshold(interest_work_2, interest, 120, 240, cv::THRESH_BINARY);

		std::vector<std::vector<cv::Point>> contours;
		std::vector<std::vector<cv::Point>> contours_useful;
		std::vector<cv::Point> hierarchy;
		findContours(interest, contours, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);
		cv::Mat imageContours = cv::Mat::zeros(interest.size(), CV_8UC1);
		cv::Mat Contours = cv::Mat::zeros(interest.size(), CV_8UC1);

		for (int i = 0; i < contours.size(); i++)
		{
			if (std::count_if(contours[i].begin(), contours[i].end(), [](cv::Point &pt) {return (pt.x == 0 || pt.y == 0); }) > 0)
			{
				continue;
			}
			else
			{
				contours_useful.push_back(contours[i]);
			}
		}

		std::vector<std::vector<cv::Point>> contours_ploy(contours_useful.size());
		std::vector<std::vector<cv::Point>> contours_filtered;
		std::vector<cv::Point> contours_original;

		for (int i = 0; i < contours_useful.size(); i++)
		{
			approxPolyDP(contours_useful[i], contours_ploy[i], 10, true);
		}

		for (int i = 0; i < contours_ploy.size(); i++)
		{
			if (contours_ploy[i].size() < 3)
			{
				continue;
			}

			if (cv::contourArea(contours_ploy[i]) < 15)
			{
				continue;
			}

			contours_filtered.push_back(contours_ploy[i]);
		}

		if (contours_filtered.size() > 1)
		{

			auto min_contours_group = std::min_element(contours_filtered.begin(), contours_filtered.end(), [&center_x, &center_y](std::vector<cv::Point> &p1, std::vector<cv::Point> &p2)
				{
					auto ploy_monment1 = cv::moments(p1);
					auto cx1 = static_cast<float>(ploy_monment1.m10 / ploy_monment1.m00);
					auto cy1 = static_cast<float>(ploy_monment1.m01 / ploy_monment1.m00);

					auto ploy_monment2 = cv::moments(p2);
					auto cx2 = static_cast<float>(ploy_monment2.m10 / ploy_monment2.m00);
					auto cy2 = static_cast<float>(ploy_monment2.m01 / ploy_monment2.m00);

					float dist1 = sqrt((cx1 - center_x)*(cx1 - center_x) + (cy1 - center_y)*(cy1 - center_y));
					float dist2 = sqrt((cx2 - center_x)*(cx2 - center_x) + (cy2 - center_y)*(cy2 - center_y));

					return dist1 < dist2;
				});

			auto &final_group = *min_contours_group;
			for (int i = 0; i < final_group.size(); i++)
			{
				contours_original.emplace_back(final_group[i].x + left_bounds, final_group[i].y + up_bounds);
			}
		}
		else if (contours_filtered.size() == 1)
		{
			for (int i = 0; i < contours_filtered[0].size(); i++)
			{
				contours_original.emplace_back(contours_filtered[0][i].x + left_bounds, contours_filtered[0][i].y + up_bounds);
			}
		}
		else
		{
			return;
		}


		for (int i = 0; i < corner.p.size(); i++)
		{
			double px = corner.p[i].x;
			double py = corner.p[i].y;
			for (int j = 0; j < contours_original.size(); j++)
			{
				int cx = contours_original[j].x;
				int cy = contours_original[j].y;
				float dist = sqrt((px - cx)*(px - cx) + (py - cy)*(py - cy));
				if (dist < 10)
				{
					black_box_pts.emplace_back(px, py);
					break;
				}
			}
		}
	}

	void filter_black_boxes(CameraPosition camera_position, const cv::Mat& image, chessboardObject& pts, Corner& corner, std::vector<Board>& boards)
	{
		int left_left_position, left_right_position, right_left_position, right_right_position;
		int left_up_position, left_under_position, right_up_position, right_under_position;



		if (camera_position == CameraPosition::LEFT)
		{
			left_left_position = 100;
			left_right_position = 220;
			right_left_position = 920;
			right_right_position = 1110;
			left_up_position = 390;
			left_under_position = 520;
			right_up_position = 420;
			right_under_position = 600;
		}
		else if (camera_position == CameraPosition::RIGHT)
		{
			left_left_position = 180;
			left_right_position = 330;
			right_left_position = 1040;
			right_right_position = 1200;
			left_up_position = 470;
			left_under_position = 620;
			right_up_position = 340;
			right_under_position = 480;
		}
		else if (camera_position == CameraPosition::FRONT)
		{
			left_left_position = 140;
			left_right_position = 380;
			right_left_position = 850;
			right_right_position = 1090;
			left_up_position = 400;
			left_under_position = 540;
			right_up_position = 400;
			right_under_position = 540;
		}
		else if (camera_position == CameraPosition::BACK)
		{
			left_left_position = 150;
			left_right_position = 340;
			right_left_position = 950;
			right_right_position = 1160;
			left_up_position = 400;
			left_under_position = 540;
			right_up_position = 340;
			right_under_position = 500;
		}


		std::vector<cv::Point2f> left_box, right_box;

		int kernel_width = 4, kernel_height = 9;

		if (camera_position == CameraPosition::FRONT || camera_position == CameraPosition::BACK)
		{
			kernel_height = 9;
			kernel_width = 9;
		}
		else
		{
			kernel_height = 4;
			kernel_width = 4;
		}

		find_pts_on_black_boxes(left_left_position, left_right_position, left_up_position, left_under_position, kernel_width, kernel_height, image, corner, left_box);
		find_pts_on_black_boxes(right_left_position, right_right_position, right_up_position, right_under_position, kernel_width, kernel_height, image, corner, right_box);




		//cv::Mat image_draw = image.clone();
		//for (int i = 0; i < left_box.size(); i++)
		//{
		//	cv::circle(image_draw, left_box[i], 2, cv::Scalar(0, 200, 100), 3);
		//}

		//for (int i = 0; i < right_box.size(); i++)
		//{
		//	cv::circle(image_draw, right_box[i], 2, cv::Scalar(0, 200, 100), 3);
		//}
		//
		//cv::imshow("746", image_draw);
		//cv::waitKey(5);


		int board_x_size = 6, board_y_size = 4;
		for (int i = 0; i < boards.size(); i++)
		{
			if (boards[i].position == BoardPosition::MiddleBoard)
			{
				board_y_size = boards[i].idx.size();
				board_x_size = boards[i].idx[0].size();
			}
		}
			   
		std::vector<cv::Point2f> top, bottom, middle;
		auto& row2 = pts[camera_position][cbdetect::BoardPosition::MiddleBoard];
		float cell_y = 20;

		for (int i = 0; i < board_x_size; i++)
		{
			bool written = false;
			for (int j = 0; j < board_y_size; j++)
			{
				auto index1 = cv::Point2i(i, j);
				auto index2 = cv::Point2i(i, j + 1);
				if (row2.count(index1) && row2.count(index2))
				{
					cell_y = abs(row2[index1].y - row2[index2].y);
					written = true;
					break;
				}
			}
			if (written)
			{
				break;
			}
		}

		for (int i = 0; i < board_x_size; i++)
		{

			auto index = cv::Point2i(i, 3);
			if (row2.count(index))
			{
				top.emplace_back(row2[index].x, row2[index].y - cell_y / 4.5f);
			}

			auto index2 = cv::Point2i(i, 0);
			if (row2.count(index2))
			{
				bottom.emplace_back(row2[index2].x, row2[index2].y + cell_y / 2.5f);
			}

			auto index3 = cv::Point2i(i, 1);
			if (row2.count(index3))
			{
				middle.emplace_back(row2[index3].x, row2[index3].y);
			}
		}


		auto fits_top = cbdetect::chessboard_curve_fit(top);
		auto fits_bottom = cbdetect::chessboard_curve_fit(bottom);
		auto fits_middle = chessboard_curve_fit(middle);

		auto middle_check = [&fits_middle](cv::Point2f & p)
		{
			float middle_p_y = fits_middle(2) * p.x * p.x + fits_middle(1) * p.x + fits_middle(0);
			if (p.y > middle_p_y)
			{
				return +1;
			}
			else
			{
				return -1;
			}
		};



		//cv::Mat image4 = image.clone();
		//for (int i = 0; i < 1280; i++)
		//{
		//	float fx = i;
		//	float fy = fits_middle(2)*fx*fx + fits_middle(1)*fx + fits_middle(0);
		//	float fy2 = fits_top(2)*fx*fx + fits_top(1)*fx + fits_top(0);
		//	float fy3 = fits_bottom(2)*fx*fx + fits_bottom(1)*fx + fits_bottom(0);
		//	cv::circle(image4, cv::Point2f(fx, fy), 1, cv::Scalar(0, 200, 0), 3);
		//	cv::circle(image4, cv::Point2f(fx, fy2), 1, cv::Scalar(0, 200, 0), 3);
		//	cv::circle(image4, cv::Point2f(fx, fy3), 1, cv::Scalar(0, 200, 0), 3);
		//}

		//cv::imshow("tesa", image4);
		//cv::waitKey();




		std::vector<cv::Point2f> top_two, bottom_two;
		std::vector<int> top_left_right, bottom_left_right;
		for (int i = 0; i < right_box.size(); i++)
		{
			if (middle_check(right_box[i]) > 0)
			{
				bottom_two.push_back(right_box[i]);
			}
			else
			{
				top_two.push_back(right_box[i]);
			}
		}

		if (top_two.size() > 1)
		{
			if (top_two[0].x > top_two[1].x)
			{
				pts[camera_position][BoardPosition::RightBoard][cv::Point2i(0, 1)] = top_two[1];
				pts[camera_position][BoardPosition::RightBoard][cv::Point2i(1, 1)] = top_two[0];
			}
			else
			{
				pts[camera_position][BoardPosition::RightBoard][cv::Point2i(0, 1)] = top_two[0];
				pts[camera_position][BoardPosition::RightBoard][cv::Point2i(1, 1)] = top_two[1];
			}
		}

		if (bottom_two.size() > 1)
		{
			if (bottom_two[0].x > bottom_two[1].x)
			{
				pts[camera_position][BoardPosition::RightBoard][cv::Point2i(0, 0)] = bottom_two[1];
				pts[camera_position][BoardPosition::RightBoard][cv::Point2i(1, 0)] = bottom_two[0];
			}
			else
			{
				pts[camera_position][BoardPosition::RightBoard][cv::Point2i(0, 0)] = bottom_two[0];
				pts[camera_position][BoardPosition::RightBoard][cv::Point2i(1, 0)] = bottom_two[1];
			}
		}


		top_two.clear();
		bottom_two.clear();

		for (int i = 0; i < left_box.size(); i++)
		{
			if (middle_check(left_box[i]) > 0)
			{
				bottom_two.push_back(left_box[i]);
			}
			else
			{
				top_two.push_back(left_box[i]);
			}
		}

		if (top_two.size() > 1)
		{
			if (top_two[0].x > top_two[1].x)
			{
				pts[camera_position][BoardPosition::LeftBoard][cv::Point2i(0, 1)] = top_two[1];
				pts[camera_position][BoardPosition::LeftBoard][cv::Point2i(1, 1)] = top_two[0];
			}
			else
			{
				pts[camera_position][BoardPosition::LeftBoard][cv::Point2i(0, 1)] = top_two[0];
				pts[camera_position][BoardPosition::LeftBoard][cv::Point2i(1, 1)] = top_two[1];
			}
		}

		if (bottom_two.size() > 1)
		{
			if (bottom_two[0].x > bottom_two[1].x)
			{
				pts[camera_position][BoardPosition::LeftBoard][cv::Point2i(0, 0)] = bottom_two[1];
				pts[camera_position][BoardPosition::LeftBoard][cv::Point2i(1, 0)] = bottom_two[0];
			}
			else
			{
				pts[camera_position][BoardPosition::LeftBoard][cv::Point2i(0, 0)] = bottom_two[0];
				pts[camera_position][BoardPosition::LeftBoard][cv::Point2i(1, 0)] = bottom_two[1];
			}
		}

	}
} // namespace cbdetect
