
#include <vector>

#include <opencv2/opencv.hpp>

#include "libcbdetect/plot_boards.h"
#include "libcbdetect/config.h"

namespace cbdetect
{

	void plot_boards(const cv::Mat &img, const Corner &corners,
					 const std::vector<Board> &boards, const Params &params)
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

		for (int n = 0; n < boards.size(); ++n)
		{
			const auto &board = boards[n];

			for (int i = 1; i < board.idx.size() - 1; ++i)
			{
				for (int j = 1; j < board.idx[i].size() - 1; ++j)
				{
					if (board.idx[i][j] < 0)
					{
						continue;
					}
					// plot lines in color
					if (board.idx[i][j + 1] >= 0)
					{
						cv::line(img_show, corners.p[board.idx[i][j]], corners.p[board.idx[i][j + 1]],
								 cv::Scalar(0, 0, 255), 3, cv::LINE_AA);
					}
					if (params.corner_type == MonkeySaddlePoint && board.idx[i + 1][j + 1] >= 0)
					{
						cv::line(img_show, corners.p[board.idx[i][j]], corners.p[board.idx[i + 1][j + 1]],
								 cv::Scalar(0, 0, 255), 3, cv::LINE_AA);
					}
					if (board.idx[i + 1][j] >= 0)
					{
						cv::line(img_show, corners.p[board.idx[i][j]], corners.p[board.idx[i + 1][j]],
								 cv::Scalar(0, 0, 255), 3, cv::LINE_AA);
					}

					// plot lines in white
					if (board.idx[i][j + 1] >= 0)
					{
						cv::line(img_show, corners.p[board.idx[i][j]], corners.p[board.idx[i][j + 1]],
								 cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
					}
					if (params.corner_type == MonkeySaddlePoint && board.idx[i + 1][j + 1] >= 0)
					{
						cv::line(img_show, corners.p[board.idx[i][j]], corners.p[board.idx[i + 1][j + 1]],
								 cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
					}
					if (board.idx[i + 1][j] >= 0)
					{
						cv::line(img_show, corners.p[board.idx[i][j]], corners.p[board.idx[i + 1][j]],
								 cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
					}
				}
			}

			// plot coordinate system
			for (int i = 1; i < board.idx.size() * board.idx[0].size(); ++i)
			{
				int row = i / board.idx[0].size();
				int col = i % board.idx[0].size();
				if (board.idx[row][col] < 0 || col == board.idx[0].size() - 1 ||
					board.idx[row][col + 1] < 0 || board.idx[row + 1][col] < 0)
				{
					continue;
				}
				cv::line(img_show, corners.p[board.idx[row][col]], corners.p[board.idx[row][col + 1]],
						 cv::Scalar(255, 0, 0), 3, cv::LINE_AA);
				cv::line(img_show, corners.p[board.idx[row][col]], corners.p[board.idx[row + 1][col]],
						 cv::Scalar(0, 255, 0), 3, cv::LINE_AA);
				break;
			}

			// plot numbers
			cv::Point2d mean(0.0, 0.0);
			for (int i = 1; i < board.idx.size() - 1; ++i)
			{
				for (int j = 1; j < board.idx[i].size() - 1; ++j)
				{
					if (board.idx[i][j] < 0)
					{
						continue;
					}
					mean += corners.p[board.idx[i][j]];
				}
			}
			mean /= (double)(board.num);
			mean.x -= 10;
			mean.y += 10;
			cv::putText(img_show, std::to_string(n), mean,
						cv::FONT_HERSHEY_SIMPLEX, 1.3, cv::Scalar(196, 196, 0), 2);
		}

		cv::imshow("boards_img", img_show);
		cv::waitKey(10);
	}
	void plot_boards(const cv::Mat &img, const Corner &corners,
					 const std::vector<Board> &boards, const Params &params,
					 const std::string &windowname)
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

		for (int n = 0; n < boards.size(); ++n)
		{
			const auto &board = boards[n];

			for (int i = 1; i < board.idx.size() - 1; ++i)
			{
				for (int j = 1; j < board.idx[i].size() - 1; ++j)
				{
					if (board.idx[i][j] < 0)
					{
						continue;
					}
					// plot lines in color
					if (board.idx[i][j + 1] >= 0)
					{
						cv::line(img_show, corners.p[board.idx[i][j]], corners.p[board.idx[i][j + 1]],
								 cv::Scalar(0, 0, 255), 3, cv::LINE_AA);
					}
					if (params.corner_type == MonkeySaddlePoint && board.idx[i + 1][j + 1] >= 0)
					{
						cv::line(img_show, corners.p[board.idx[i][j]], corners.p[board.idx[i + 1][j + 1]],
								 cv::Scalar(0, 0, 255), 3, cv::LINE_AA);
					}
					if (board.idx[i + 1][j] >= 0)
					{
						cv::line(img_show, corners.p[board.idx[i][j]], corners.p[board.idx[i + 1][j]],
								 cv::Scalar(0, 0, 255), 3, cv::LINE_AA);
					}

					// plot lines in white
					if (board.idx[i][j + 1] >= 0)
					{
						cv::line(img_show, corners.p[board.idx[i][j]], corners.p[board.idx[i][j + 1]],
								 cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
					}
					if (params.corner_type == MonkeySaddlePoint && board.idx[i + 1][j + 1] >= 0)
					{
						cv::line(img_show, corners.p[board.idx[i][j]], corners.p[board.idx[i + 1][j + 1]],
								 cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
					}
					if (board.idx[i + 1][j] >= 0)
					{
						cv::line(img_show, corners.p[board.idx[i][j]], corners.p[board.idx[i + 1][j]],
								 cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
					}
				}
			}

			// plot coordinate system
			for (int i = 1; i < board.idx.size() * board.idx[0].size(); ++i)
			{
				int row = i / board.idx[0].size();
				int col = i % board.idx[0].size();
				if (board.idx[row][col] < 0 || col == board.idx[0].size() - 1 ||
					board.idx[row][col + 1] < 0 || board.idx[row + 1][col] < 0)
				{
					continue;
				}
				cv::line(img_show, corners.p[board.idx[row][col]], corners.p[board.idx[row][col + 1]],
						 cv::Scalar(255, 0, 0), 3, cv::LINE_AA);
				cv::line(img_show, corners.p[board.idx[row][col]], corners.p[board.idx[row + 1][col]],
						 cv::Scalar(0, 255, 0), 3, cv::LINE_AA);
				break;
			}

			// plot numbers
			cv::Point2d mean(0.0, 0.0);
			for (int i = 1; i < board.idx.size() - 1; ++i)
			{
				for (int j = 1; j < board.idx[i].size() - 1; ++j)
				{
					if (board.idx[i][j] < 0)
					{
						continue;
					}
					mean += corners.p[board.idx[i][j]];
				}
			}
			mean /= (double)(board.num);
			mean.x -= 10;
			mean.y += 10;
			cv::putText(img_show, std::to_string(n), mean,
						cv::FONT_HERSHEY_SIMPLEX, 1.3, cv::Scalar(196, 196, 0), 2);
		}

		cv::imshow(windowname, img_show);
		cv::waitKey(10);
	}
	void drawArrow(cv::Mat &img, cv::Point pStart, cv::Point pEnd, int len, int alpha, cv::Scalar color, int thickness, int lineType)
	{
		const double PI = 3.1415926;
		cv::Point arrow;
		double angle = atan2((double)(pStart.y - pEnd.y), (double)(pStart.x - pEnd.x));
		cv::line(img, pStart, pEnd, color, thickness, lineType);
		arrow.x = pEnd.x + len * cos(angle + PI * alpha / 180);
		arrow.y = pEnd.y + len * sin(angle + PI * alpha / 180);
		cv::line(img, pEnd, arrow, color, thickness, lineType);
		arrow.x = pEnd.x + len * cos(angle - PI * alpha / 180);
		arrow.y = pEnd.y + len * sin(angle - PI * alpha / 180);
		cv::line(img, pEnd, arrow, color, thickness, lineType);
	}

	void my_plot_boards(CameraPosition pos, const cv::Mat &img, const Corner &corners, const std::vector<Board> &boards, const Params &params, const std::string &path, int num)
	{
		bool write_image = false;
		if (path != " " && num > 0)
		{
			write_image = true;
		}

		cv::Mat img_show;

		if (img.channels() != 3)
		{
			cv::cvtColor(img, img_show, cv::COLOR_GRAY2BGR);
		}
		else
		{
			img_show = img.clone();
		}

		cv::Mat img_show2 = img_show.clone();

		for (int n = 0; n < boards.size(); ++n)
		{
			const auto &board = boards[n];

			double y_space = 0, x_space = 0;
			int x_counter = 0, y_counter = 0;

			for (int i = 1; i < board.idx.size() - 1; ++i)
			{
				for (int j = 1; j < board.idx[i].size() - 1; ++j)
				{

					if (board.idx[i][j] < 0)
					{
						continue;
					}

					// plot lines in color
					if (board.idx[i][j + 1] >= 0)
					{
						cv::line(img_show, corners.p[board.idx[i][j]], corners.p[board.idx[i][j + 1]], cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
						x_space += (corners.p[board.idx[i][j + 1]].x - corners.p[board.idx[i][j]].x);
						x_counter++;
					}
					if (board.idx[i + 1][j] >= 0)
					{
						cv::line(img_show, corners.p[board.idx[i][j]], corners.p[board.idx[i + 1][j]], cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
						y_space += corners.p[board.idx[i + 1][j]].y - corners.p[board.idx[i][j]].y;
						y_counter++;
					}
				}
			}

			// plot coordinate system
			for (int i = 1; i < board.idx.size() * board.idx[0].size(); ++i)
			{
				int row = i / board.idx[0].size();
				int col = i % board.idx[0].size();

				if (board.idx[row][col] < 0 || col == board.idx[0].size() - 1 || board.idx[row][col + 1] < 0 || board.idx[row + 1][col] < 0)
				{
					continue;
				}

				drawArrow(img_show, corners.p[board.idx[row][col]], corners.p[board.idx[row][col + 1]], 20, 15, cv::Scalar(250, 255, 0), 1, cv::LINE_AA);
				drawArrow(img_show, corners.p[board.idx[row][col]], corners.p[board.idx[row + 1][col]], 20, 15, cv::Scalar(0, 255, 0), 1, cv::LINE_AA);

				break;
			}

			// plot numbers
			cv::Point2d mean(0.0, 0.0);
			for (int i = 1; i < board.idx.size() - 1; ++i)
			{
				for (int j = 1; j < board.idx[i].size() - 1; ++j)
				{
					if (board.idx[i][j] < 0)
					{
						continue;
					}
					mean += corners.p[board.idx[i][j]];
				}
			}
			mean /= (double)(board.num);
			mean.x -= 10;
			mean.y += 10;
			cv::putText(img_show, std::to_string(n), mean, cv::FONT_HERSHEY_SIMPLEX, 1.3, cv::Scalar(196, 196, 0), 2);

			// plot object
			for (int i = 1; i < board.idx.size() - 1; ++i)
			{
				for (int j = 1; j < board.idx[i].size() - 1; ++j)
				{
					if (board.objectInverseIdx.size() > 0)
					{
						int ox = board.objectInverseIdx[i][j].x;
						int oy = board.objectInverseIdx[i][j].y;

						if (ox > 0 && oy > 0)
						{
							int opx = board.objectIdx[oy][ox].x;
							int opy = board.objectIdx[oy][ox].y;
							cv::putText(img_show2, std::to_string(opx) + std::to_string(opy), corners.p[board.idx[oy][ox]], cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1);
						}
					}
				}
			}
		}

		cv::imshow("boards_img", img_show);
		cv::imshow("boards_img2", img_show2);

		if (write_image)
		{
			if (pos == CameraPosition::LEFT)
			{
				cv::imwrite(path + std::to_string(num) + "left.jpg", img_show);
				cv::imwrite(path + "o" + std::to_string(num) + "left.jpg", img_show2);
			}
			if (pos == CameraPosition::RIGHT)
			{
				cv::imwrite(path + std::to_string(num) + "right.jpg", img_show);
				cv::imwrite(path + "o" + std::to_string(num) + "right.jpg", img_show2);
			}
			if (pos == CameraPosition::FRONT)
			{
				cv::imwrite(path + std::to_string(num) + "front.jpg", img_show);
				cv::imwrite(path + "o" + std::to_string(num) + "front.jpg", img_show2);
			}
			if (pos == CameraPosition::BACK)
			{
				cv::imwrite(path + std::to_string(num) + "back.jpg", img_show);
				cv::imwrite(path + "o" + std::to_string(num) + "back.jpg", img_show2);
			}
		}

		cv::waitKey(5);
	}

	void getAlignedImageAndPatternPoints(const Corner &corners, const std::vector<Board> &boards, cv::Mat &objectPoints, cv::Mat &imagePoints, double dx, double dy)
	{
		std::vector<cv::Point2d> _imagePoints;
		std::vector<cv::Point3d> _objectPoints;

		for (int n = 0; n < boards.size(); ++n)
		{
			const auto &board = boards[n];

			for (int i = 1; i < board.idx.size() - 1; ++i)
			{
				for (int j = 1; j < board.idx[i].size() - 1; ++j)
				{
					if (board.idx[i][j] < 0)
					{
						continue;
					}
					_imagePoints.push_back(corners.p[board.idx[i][j]]);
					cv::Point3d objPt;
					// double dx = 30;//mm, the size of the chessboard block
					// double dy = 30;//mm
					objPt.x = (i - 1) * dx;
					objPt.y = (j - 1) * dy;
					objPt.z = 0;

					_objectPoints.push_back(objPt);
				}
			}
		}

		// cv::Mat o_tmp(_objectPoints);
		objectPoints = cv::Mat(_objectPoints).clone();
		imagePoints = cv::Mat(_imagePoints).clone();
	}

} // namespace cbdetect
