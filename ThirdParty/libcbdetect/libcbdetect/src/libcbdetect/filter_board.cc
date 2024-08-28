
#include <vector>
#include <memory>
#include <opencv2/opencv.hpp>
#include <Eigen/Dense>


#include "libcbdetect/board_energy.h"
#include "libcbdetect/config.h"
#include "libcbdetect/filter_board.h"
#include "libcbdetect/plot_boards.h"

namespace cbdetect {

double find_minE(const Board& board, const cv::Point2i& p) {
  double minE = std::min(std::min(board.energy[p.y][p.x][0], board.energy[p.y][p.x][1]),
                         board.energy[p.y][p.x][2]);
  if(p.x - 1 >= 0) {
    minE = std::min(minE, board.energy[p.y][p.x - 1][0]);
  }
  if(p.x - 1 >= 0 && p.y - 1 >= 0) {
    minE = std::min(minE, board.energy[p.y - 1][p.x - 1][1]);
  }
  if(p.y - 1 >= 0) {
    minE = std::min(minE, board.energy[p.y - 1][p.x][2]);
  }
  if(p.x - 2 >= 0) {
    minE = std::min(minE, board.energy[p.y][p.x - 2][0]);
  }
  if(p.x - 2 >= 0 && p.y - 2 >= 0) {
    minE = std::min(minE, board.energy[p.y - 2][p.x - 2][1]);
  }
  if(p.y - 2 >= 0) {
    minE = std::min(minE, board.energy[p.y - 2][p.x][2]);
  }
  return minE;
}

void filter_board(const Corner& corners, std::vector<int>& used, Board& board,
                  std::vector<cv::Point2i>& proposal, double& energy, const Params& params) {
  // erase wrong corners
  while(!proposal.empty()) {
    cv::Point3i maxE_pos = board_energy(corners, board, params);
    double p_energy      = board.energy[maxE_pos.y][maxE_pos.x][maxE_pos.z];
    if(p_energy <= energy) {
      energy = p_energy;
      break;
    }
    if(params.corner_type == SaddlePoint && !params.occlusion) {
      for(const auto& p : proposal) {
        used[board.idx[p.y][p.x]] = 0;
        board.idx[p.y][p.x]       = -2;
        --board.num;
      }
      return;
    }

    // find the wrongest corner
    cv::Point2i p[3];
    p[0] = {maxE_pos.x, maxE_pos.y};
    switch(maxE_pos.z) {
    case 0: {
      p[1] = {maxE_pos.x + 1, maxE_pos.y};
      p[2] = {maxE_pos.x + 2, maxE_pos.y};
      break;
    }
    case 1: {
      p[1] = {maxE_pos.x + 1, maxE_pos.y + 1};
      p[2] = {maxE_pos.x + 2, maxE_pos.y + 2};
      break;
    }
    case 2: {
      p[1] = {maxE_pos.x, maxE_pos.y + 1};
      p[2] = {maxE_pos.x, maxE_pos.y + 2};
      break;
    }
    default:
      break;
    }
    double minE_wrong[3];
    minE_wrong[0] = find_minE(board, p[0]);
    minE_wrong[1] = find_minE(board, p[1]);
    minE_wrong[2] = find_minE(board, p[2]);

    double minE = -DBL_MAX;
    int iter    = 0;
    for(auto it = proposal.begin(); it < proposal.end(); ++it) {
      if(it->x == p[0].x && it->y == p[0].y && minE_wrong[0] > minE) {
        minE       = minE_wrong[0];
        maxE_pos.x = it->x;
        maxE_pos.y = it->y;
        iter       = it - proposal.begin();
      }
      if(it->x == p[1].x && it->y == p[1].y && minE_wrong[1] > minE) {
        minE       = minE_wrong[1];
        maxE_pos.x = it->x;
        maxE_pos.y = it->y;
        iter       = it - proposal.begin();
      }
      if(it->x == p[2].x && it->y == p[2].y && minE_wrong[2] > minE) {
        minE       = minE_wrong[2];
        maxE_pos.x = it->x;
        maxE_pos.y = it->y;
        iter       = it - proposal.begin();
      }
    }

    proposal.erase(proposal.begin() + iter);
    used[board.idx[maxE_pos.y][maxE_pos.x]] = 0;
    board.idx[maxE_pos.y][maxE_pos.x]       = -2;
    --board.num;
  }
}


	void temp_convert(Board &board)
	{
		int board_y_size = board.idx.size();
		int board_x_size = board.idx[0].size();


		std::vector<std::vector<int>> newIdx(board_x_size, std::vector<int>(board_y_size, -1));

		for (int y = 0; y < board_y_size; y++)
		{
			for (int x = 0; x < board_x_size; x++)
			{
				newIdx[x][y] = board.idx[board_y_size - 1 - y][x];
			}
		}

		board.idx.clear();
		board.idx = newIdx;

	}

	static int judge_quadrant(double x_direction, double y_direction)
	{
		Eigen::Vector2d x_axis(1, 0);
		Eigen::Vector2d y_axis(0, 1);
		Eigen::Vector2d direction(x_direction, y_direction);

		double x_angle = acos(x_axis.dot(direction) / (x_axis.norm()*direction.norm()));
		double y_angle = acos(y_axis.dot(direction) / (y_axis.norm()*direction.norm()));

		//std::cout << std::endl << x_angle << "," << y_angle << std::endl;
		double thr_angle = 0.05;
		if (x_angle <= (M_PI_2) && y_angle <= (M_PI_2 + thr_angle))
		{
			return 1;
		}
		else if (x_angle > (M_PI_2) && y_angle <= (M_PI_2 + thr_angle))
		{
			return 2;
		}
		else if (x_angle <= (M_PI_2) && y_angle > (M_PI_2 + thr_angle))
		{
			return 4;
		}
		else if (x_angle > (M_PI_2) && y_angle > (M_PI_2 + thr_angle))
		{
			return 3;
		}

		//if (x_direction >= 0 && y_direction >= 0)
		//{
		//	return 1;
		//}
		//else if (x_direction < 0 && y_direction >= 0)
		//{
		//	return 2;
		//}
		//else if (x_direction >= 0 && y_direction < 0)
		//{
		//	return 4;
		//}
		//else
		//{
		//	return 3;
		//}
	}

	static void fill_object_index(BoardOriginType type, Board &board)
	{
		if (type == BoardOriginType::RightUpDiff)
		{
			for (int i = 1; i < board.idx.size() - 1; ++i)
			{
				for (int j = 1; j < board.idx[i].size() - 1; ++j)
				{
					if (board.idx[i][j] > 0)
					{
						board.objectIdx[i][j] = cv::Point2i( board.idx.size() - 2 - i,board.idx[i].size() - 2 - j);
					}

				}
			}
		}
		else if (type == BoardOriginType::RightUpSame)
		{
			for (int i = 1; i < board.idx.size() - 1; ++i)
			{
				for (int j = 1; j < board.idx[i].size() - 1; ++j)
				{
					if (board.idx[i][j] > 0)
					{
						board.objectIdx[i][j] = cv::Point2i( board.idx[i].size() - 2 - j, board.idx.size() - 2 - i);
					}

				}
			}
		}
		else if (type == BoardOriginType::LeftUpDiff)
		{
			for (int i = 1; i < board.idx.size() - 1; ++i)
			{
				for (int j = 1; j < board.idx[i].size() - 1; ++j)
				{
					if (board.idx[i][j] > 0)
					{
						board.objectIdx[i][j] = cv::Point2i(board.idx.size() - 2 - i,j - 1);
						//board.objectIdx[i][j] = cv::Point2i(board.idx[i].size() - 2 - j, i - 1);
					}
				}
			}
		}
		else if (type == BoardOriginType::LeftUpSame)
		{
			for (int i = 1; i < board.idx.size() - 1; ++i)
			{
				for (int j = 1; j < board.idx[i].size() - 1; ++j)
				{
					if (board.idx[i][j] > 0)
					{
						board.objectIdx[i][j] = cv::Point2i(j - 1, board.idx.size() - 2 - i);
						//board.objectIdx[i][j] = cv::Point2i(board.idx[i].size() - 2 - j, i - 1);
					}
				}
			}
		}
		else if (type == BoardOriginType::RightDownDiff)
		{
			for (int i = 1; i < board.idx.size() - 1; ++i)
			{
				for (int j = 1; j < board.idx[i].size() - 1; ++j)
				{
					if (board.idx[i][j] > 0)
					{
						board.objectIdx[i][j] = cv::Point2i( i - 1, board.idx[i].size() - 2 - j);
					}
				}
			}
		}
		else if (type == BoardOriginType::RightDownSame)
		{
			for (int i = 1; i < board.idx.size() - 1; ++i)
			{
				for (int j = 1; j < board.idx[i].size() - 1; ++j)
				{
					if (board.idx[i][j] > 0)
					{
						board.objectIdx[i][j] = cv::Point2i(board.idx[i].size() - 2 - j, i - 1);
					}
				}
			}
		}
		else if (type == BoardOriginType::LeftDownDiff)
		{
			for (int i = 1; i < board.idx.size() - 1; ++i)
			{
				for (int j = 1; j < board.idx[i].size() - 1; ++j)
				{
					if (board.idx[i][j] > 0)
					{
						board.objectIdx[i][j] = cv::Point2i(i - 1, j - 1);
					}
				}
			}
		}
		else if (type == BoardOriginType::LeftDownSame)
		{
			for (int i = 1; i < board.idx.size() - 1; ++i)
			{
				for (int j = 1; j < board.idx[i].size() - 1; ++j)
				{
					if (board.idx[i][j] > 0)
					{
						board.objectIdx[i][j] = cv::Point2i(j - 1, i - 1);
					}
				}
			}
		}



		for (int i = 1; i < board.idx.size() - 1; ++i)
		{
			for (int j = 1; j < board.idx[i].size() - 1; ++j)
			{
				cv::Point2i iIndex = board.objectIdx[i][j];
				board.objectInverseIdx[iIndex.y + 1][iIndex.x + 1] = cv::Point2i(j, i);
			}
		}
	}



	void refine_board(CameraPosition camera_postion_label, const Corner& corners, std::vector<Board> &boards, cv::Mat &image)
	{
		int numberBoard = boards.size();
		double image_width = image.cols;
		double image_height = image.rows;

		if (camera_postion_label == CameraPosition::LEFT || camera_postion_label == CameraPosition::FRONT || camera_postion_label == CameraPosition::BACK || camera_postion_label == CameraPosition::RIGHT)
		{
			std::vector<std::pair<int, cv::Point2d>> centers(boards.size());
			for (int n = 0; n < boards.size(); ++n)
			{
				auto& board = boards[n];

				cv::Point2d center(0.0, 0.0);
				int center_counter = 0;

				for (int i = 1; i < board.idx.size() - 1; ++i)
				{
					for (int j = 1; j < board.idx[i].size() - 1; ++j)
					{
						if (board.idx[i][j] > 0)
						{
							center.x = center.x + corners.p[board.idx[i][j]].x;
							center.y = center.y + corners.p[board.idx[i][j]].y;
							center_counter++;
						}
					}
				}

				center.x = center.x / center_counter;
				center.y = center.y / center_counter;
				centers[n] = std::make_pair(n, center);
			}

			//std::sort(centers.begin(), centers.end(), [](std::pair<int, cv::Point2d>& p1, std::pair<int, cv::Point2d>& p2)
			//	{
			//		return (p1.second.x > p2.second.x);
			//	});

			for (int i = 0; i < centers.size(); i++)
			{
				double center_x = centers[i].second.x;
				if (center_x > (image_width / 2 - 150) && center_x < (image_width / 2 + 150))
				{
					boards[centers[i].first].position = BoardPosition::MiddleBoard;
					boards[centers[i].first].center = centers[i].second;
				}
				else if (center_x <= (image_width / 2 - 150))
				{
					boards[centers[i].first].position = BoardPosition::LeftBoard;
					boards[centers[i].first].center = centers[i].second;
				}
				else
				{
					boards[centers[i].first].position = BoardPosition::RightBoard;
					boards[centers[i].first].center = centers[i].second;
				}

			}

			for (int n = 0; n < boards.size(); ++n)
			{
				auto& board = boards[n];




				if (board.position == BoardPosition::MiddleBoard)
				{

					std::vector<cv::Point2d> points;

					int board_y_size = board.idx.size();
					int board_x_size = board.idx[0].size();

					if (board_y_size > board_x_size)
					{
						std::vector<std::vector<int>> newIdx(board_x_size, std::vector<int>(board_y_size, -1));

						for (int y = 0; y < board_y_size; y++)
						{
							for (int x = 0; x < board_x_size; x++)
							{
								newIdx[x][y] = board.idx[y][x];
							}
						}

						board.idx.clear();
						board.idx = newIdx;
					}

					double x_direction, y_direction;

					for (int i = 1; i < board.idx.size() * board.idx[0].size(); ++i)
					{
						int row = i / board.idx[0].size();
						int col = i % board.idx[0].size();

						if (board.idx[row][col] < 0 || col == board.idx[0].size() - 1 || board.idx[row][col + 1] < 0 || board.idx[row + 1][col] < 0)
						{
							continue;
						}

						x_direction = corners.p[board.idx[row][col + 1]].x - corners.p[board.idx[row][col]].x;
						y_direction = corners.p[board.idx[row + 1][col]].y - corners.p[board.idx[row][col]].y;
						break;
					}

					board.objectIdx.resize(board.idx.size(), std::vector<cv::Point2i>(board.idx[0].size(), cv::Point2i(-1, -1)));
					board.objectInverseIdx.resize(board.idx.size(), std::vector<cv::Point2i>(board.idx[0].size(), cv::Point2i(-1, -1)));

					if (x_direction < 0 && y_direction > 0)
					{
						for (int i = 1; i < board.idx.size() - 1; ++i)
						{
							for (int j = 1; j < board.idx[i].size() - 1; ++j)
							{
								if (board.idx[i][j] > 0)
								{
									board.objectIdx[i][j] = cv::Point2i(board.idx[i].size() - 2 - j, board.idx.size() - 2 - i);
								}

							}
						}
					}
					else if (x_direction > 0 && y_direction > 0)
					{
						for (int i = 1; i < board.idx.size() - 1; ++i)
						{
							for (int j = 1; j < board.idx[i].size() - 1; ++j)
							{
								if (board.idx[i][j] > 0)
								{
									board.objectIdx[i][j] = cv::Point2i(j - 1, board.idx.size() - 2 - i);
									//board.objectIdx[i][j] = cv::Point2i(board.idx[i].size() - 2 - j, i - 1);
								}
							}
						}
					}
					else if (x_direction < 0 && y_direction < 0)
					{
						for (int i = 1; i < board.idx.size() - 1; ++i)
						{
							for (int j = 1; j < board.idx[i].size() - 1; ++j)
							{
								if (board.idx[i][j] > 0)
								{
									board.objectIdx[i][j] = cv::Point2i(board.idx[i].size() - 2 - j, i - 1);
								}
							}
						}
					}
					else if (x_direction > 0 && y_direction < 0)
					{
						for (int i = 1; i < board.idx.size() - 1; ++i)
						{
							for (int j = 1; j < board.idx[i].size() - 1; ++j)
							{
								if (board.idx[i][j] > 0)
								{
									board.objectIdx[i][j] = cv::Point2i(j - 1, i - 1);
								}
							}
						}
					}

					for (int i = 1; i < board.idx.size() - 1; ++i)
					{
						for (int j = 1; j < board.idx[i].size() - 1; ++j)
						{
							cv::Point2i iIndex = board.objectIdx[i][j];
							board.objectInverseIdx[iIndex.y + 1][iIndex.x + 1] = cv::Point2i(j, i);
						}
					}
				}
				else if (board.position == BoardPosition::LeftBoard)
				{
					cv::Point2d x_direction_start, x_direction_end, y_direction_start, y_direction_end;

					for (int i = 1; i < board.idx.size() * board.idx[0].size(); ++i)
					{
						int row = i / board.idx[0].size();
						int col = i % board.idx[0].size();

						if (board.idx[row][col] < 0 || col == board.idx[0].size() - 1 || board.idx[row][col + 1] < 0 || board.idx[row + 1][col] < 0)
						{
							continue;
						}

						x_direction_start = corners.p[board.idx[row][col]];
						x_direction_end = corners.p[board.idx[row][col + 1]];
						y_direction_start = corners.p[board.idx[row][col]];
						y_direction_end = corners.p[board.idx[row + 1][col]];
						break;
					}

					board.objectIdx.resize(board.idx.size(), std::vector<cv::Point2i>(board.idx[0].size(), cv::Point2i(-1, -1)));
					board.objectInverseIdx.resize(board.idx.size(), std::vector<cv::Point2i>(board.idx[0].size(), cv::Point2i(-1, -1)));

					int x_quadrant = judge_quadrant(x_direction_end.x - x_direction_start.x, x_direction_end.y - x_direction_start.y);
					int y_quadrant = judge_quadrant(y_direction_end.x - y_direction_start.x, y_direction_end.y - y_direction_start.y);

					//std::cout << x_direction_start << std::endl << x_direction_end << std::endl;
					//std::cout << y_direction_start << std::endl << y_direction_end << std::endl;

					BoardOriginType board_origin_type = BoardOriginType::LeftUpSame;

					if (x_quadrant == 1 && y_quadrant == 4)
					{
						board_origin_type = BoardOriginType::LeftDownSame;
					}
					else if (x_quadrant == 4 && y_quadrant == 1)
					{
						board_origin_type = BoardOriginType::LeftDownDiff;
					}
					else if (x_quadrant == 1 && y_quadrant == 2)
					{
						board_origin_type = BoardOriginType::LeftUpSame;
					}
					else if (x_quadrant == 2 && y_quadrant == 1)
					{
						board_origin_type = BoardOriginType::LeftUpDiff;
					}
					else if (x_quadrant == 3 && y_quadrant == 4)
					{
						board_origin_type = BoardOriginType::RightDownSame;
					}
					else if (x_quadrant == 4 && y_quadrant == 3)
					{
						board_origin_type = BoardOriginType::RightDownDiff;
					}
					else if (x_quadrant == 3 && y_quadrant == 2)
					{
						board_origin_type = BoardOriginType::RightUpSame;
					}
					else if (x_quadrant == 2 && y_quadrant == 3)
					{
						board_origin_type = BoardOriginType::RightUpDiff;
					}

					fill_object_index(board_origin_type, board);

				}

				else if (board.position == BoardPosition::RightBoard)
				{
					cv::Point2d x_direction_start, x_direction_end, y_direction_start, y_direction_end;

					for (int i = 1; i < board.idx.size() * board.idx[0].size(); ++i)
					{
						int row = i / board.idx[0].size();
						int col = i % board.idx[0].size();

						if (board.idx[row][col] < 0 || col == board.idx[0].size() - 1 || board.idx[row][col + 1] < 0 || board.idx[row + 1][col] < 0)
						{
							continue;
						}

						x_direction_start = corners.p[board.idx[row][col]];
						x_direction_end = corners.p[board.idx[row][col + 1]];
						y_direction_start = corners.p[board.idx[row][col]];
						y_direction_end = corners.p[board.idx[row + 1][col]];
						break;
					}

					board.objectIdx.resize(board.idx.size(), std::vector<cv::Point2i>(board.idx[0].size(), cv::Point2i(-1, -1)));
					board.objectInverseIdx.resize(board.idx.size(), std::vector<cv::Point2i>(board.idx[0].size(), cv::Point2i(-1, -1)));

					//std::cout << "RIGHT " << x_direction_start << " " << x_direction_end << std::endl;
					//std::cout << "RIGHT " << y_direction_start << " " << y_direction_end << std::endl;

					int x_quadrant = judge_quadrant(x_direction_end.x - x_direction_start.x, x_direction_end.y - x_direction_start.y);
					int y_quadrant = judge_quadrant(y_direction_end.x - y_direction_start.x, y_direction_end.y - y_direction_start.y);



					BoardOriginType board_origin_type = BoardOriginType::LeftUpSame;

					if (x_quadrant == 4 && y_quadrant == 3)
					{
						board_origin_type = BoardOriginType::LeftDownSame;
					}
					else if (x_quadrant == 3 && y_quadrant == 4)
					{
						board_origin_type = BoardOriginType::LeftDownDiff;
					}
					else if (x_quadrant == 4 && y_quadrant == 1)
					{
						board_origin_type = BoardOriginType::LeftUpSame;
					}
					else if (x_quadrant == 1 && y_quadrant == 4)
					{
						board_origin_type = BoardOriginType::LeftUpDiff;
					}
					else if (x_quadrant == 2 && y_quadrant == 3)
					{
						board_origin_type = BoardOriginType::RightDownSame;
					}
					else if (x_quadrant == 3 && y_quadrant == 2)
					{
						board_origin_type = BoardOriginType::RightDownDiff;
					}
					else if (x_quadrant == 2 && y_quadrant == 1)
					{
						board_origin_type = BoardOriginType::RightUpSame;
					}
					else if (x_quadrant == 1 && y_quadrant == 2)
					{
						board_origin_type = BoardOriginType::RightUpDiff;
					}

					fill_object_index(board_origin_type, board);

				}


			}
		}

		//if (camera_postion_label == CameraPosition::RIGHT)
		//{
		//	std::vector<std::pair<int, cv::Point2d>> centers(boards.size());
		//	for (int n = 0; n < boards.size(); ++n)
		//	{
		//		auto& board = boards[n];


		//		cv::Point2d center(0.0, 0.0);
		//		int center_counter = 0;

		//		for (int i = 1; i < board.idx.size() - 1; ++i)
		//		{
		//			for (int j = 1; j < board.idx[i].size() - 1; ++j)
		//			{
		//				if (board.idx[i][j] > 0)
		//				{
		//					center.x = center.x + corners.p[board.idx[i][j]].x;
		//					center.y = center.y + corners.p[board.idx[i][j]].y;
		//					center_counter++;
		//				}
		//			}
		//		}

		//		center.x = center.x / center_counter;
		//		center.y = center.y / center_counter;
		//		centers[n] = std::make_pair(n, center);
		//	}

		//	std::sort(centers.begin(), centers.end(), [](std::pair<int, cv::Point2d>& p1, std::pair<int, cv::Point2d>& p2)
		//		{
		//			return (p1.second.x > p2.second.x);
		//		});

		//	for (int i = 0; i < centers.size(); i++)
		//	{
		//		int number = centers[i].first;
		//		boards[number].position = static_cast<BoardPosition>(i);
		//		boards[number].center = centers[i].second;
		//	}

		//	for (int n = 0; n < boards.size(); ++n)
		//	{


		//		if (n == centers[1].first)
		//		{
		//			auto& board = boards[n];
		//			std::vector<cv::Point2d> points;

		//			int board_y_size = board.idx.size();
		//			int board_x_size = board.idx[0].size();

		//			if (board_y_size > board_x_size)
		//			{
		//				std::vector<std::vector<int>> newIdx(board_x_size, std::vector<int>(board_y_size, -1));

		//				for (int y = 0; y < board_y_size; y++)
		//				{
		//					for (int x = 0; x < board_x_size; x++)
		//					{
		//						newIdx[x][y] = board.idx[y][x];
		//					}
		//				}

		//				board.idx.clear();
		//				board.idx = newIdx;
		//			}

		//			double x_direction, y_direction;

		//			for (int i = 1; i < board.idx.size() * board.idx[0].size(); ++i)
		//			{
		//				int row = i / board.idx[0].size();
		//				int col = i % board.idx[0].size();

		//				if (board.idx[row][col] < 0 || col == board.idx[0].size() - 1 || board.idx[row][col + 1] < 0 || board.idx[row + 1][col] < 0)
		//				{
		//					continue;
		//				}

		//				x_direction = corners.p[board.idx[row][col + 1]].x - corners.p[board.idx[row][col]].x;
		//				y_direction = corners.p[board.idx[row + 1][col]].y - corners.p[board.idx[row][col]].y;
		//				break;
		//			}

		//			board.objectIdx.resize(board.idx.size(), std::vector<cv::Point2i>(board.idx[0].size(), cv::Point2i(-1, -1)));
		//			board.objectInverseIdx.resize(board.idx.size(), std::vector<cv::Point2i>(board.idx[0].size(), cv::Point2i(-1, -1)));

		//			if (x_direction < 0 && y_direction > 0)
		//			{
		//				for (int i = 1; i < board.idx.size() - 1; ++i)
		//				{
		//					for (int j = 1; j < board.idx[i].size() - 1; ++j)
		//					{
		//						if (board.idx[i][j] > 0)
		//						{
		//							board.objectIdx[i][j] = cv::Point2i(board.idx[i].size() - 2 - j, board.idx.size() - 2 - i);
		//						}

		//					}
		//				}
		//			}
		//			else if (x_direction > 0 && y_direction > 0)
		//			{
		//				for (int i = 1; i < board.idx.size() - 1; ++i)
		//				{
		//					for (int j = 1; j < board.idx[i].size() - 1; ++j)
		//					{
		//						if (board.idx[i][j] > 0)
		//						{
		//							board.objectIdx[i][j] = cv::Point2i(j - 1, board.idx.size() - 2 - i);
		//							//board.objectIdx[i][j] = cv::Point2i(board.idx[i].size() - 2 - j, i - 1);
		//						}
		//					}
		//				}
		//			}
		//			else if (x_direction < 0 && y_direction < 0)
		//			{
		//				for (int i = 1; i < board.idx.size() - 1; ++i)
		//				{
		//					for (int j = 1; j < board.idx[i].size() - 1; ++j)
		//					{
		//						if (board.idx[i][j] > 0)
		//						{
		//							board.objectIdx[i][j] = cv::Point2i(board.idx[i].size() - 2 - j, i - 1);
		//						}
		//					}
		//				}
		//			}
		//			else if (x_direction > 0 && y_direction < 0)
		//			{
		//				for (int i = 1; i < board.idx.size() - 1; ++i)
		//				{
		//					for (int j = 1; j < board.idx[i].size() - 1; ++j)
		//					{
		//						if (board.idx[i][j] > 0)
		//						{
		//							board.objectIdx[i][j] = cv::Point2i(j - 1, i - 1);
		//						}
		//					}
		//				}
		//			}

		//			for (int i = 1; i < board.idx.size() - 1; ++i)
		//			{
		//				for (int j = 1; j < board.idx[i].size() - 1; ++j)
		//				{
		//					cv::Point2i iIndex = board.objectIdx[i][j];
		//					board.objectInverseIdx[iIndex.y + 1][iIndex.x + 1] = cv::Point2i(j, i);
		//				}
		//			}

		//		}
		//	}
		//}
	}


	void get_board_object(cbdetect::CameraPosition position, const cbdetect::Corner& corners, std::vector<cbdetect::Board>& boards, chessboardObject &ptsWithIndex)
	{

		for (int n = 0; n < boards.size(); ++n)
		{
			const auto& board = boards[n];

			for (int i = 1; i < board.idx.size() - 1; ++i)
			{
				for (int j = 1; j < board.idx[i].size() - 1; ++j)
				{
					if (!board.objectInverseIdx.empty())
					{
						int ox = board.objectInverseIdx[i][j].x;
						int oy = board.objectInverseIdx[i][j].y;

						if (ox > 0 && oy > 0)
						{
							int opx = board.objectIdx[oy][ox].x;
							int opy = board.objectIdx[oy][ox].y;
							std::string op = std::to_string(opx) + std::to_string(opy);
							ptsWithIndex[position][board.position][cv::Point2i(opx, opy)] = corners.p[board.idx[oy][ox]];
						}
					}
				}
			}
		}
	}

} // namespace cbdetect
