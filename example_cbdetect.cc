#include <libcbdetect/boards_from_cornres.h>
#include <libcbdetect/config.h>
#include <libcbdetect/find_corners.h>
#include <libcbdetect/plot_boards.h>
#include <libcbdetect/plot_corners.h>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace std::chrono;

void detect(const char *str, cbdetect::CornerType corner_type)
{
    cbdetect::Corner corners;
    std::vector<cbdetect::Board> boards;
    cbdetect::Params params;
    params.corner_type = corner_type;

    cv::Mat img = cv::imread(str, cv::IMREAD_COLOR);

    auto t1 = high_resolution_clock::now();
    cbdetect::find_corners(img, corners, params);
    auto t2 = high_resolution_clock::now();
    cbdetect::plot_corners(img, corners);
    cv::waitKey(0);
    auto t3 = high_resolution_clock::now();
    cbdetect::boards_from_corners(img, corners, boards, params);
    auto t4 = high_resolution_clock::now();
    printf("Find corners took: %.3f ms/n", duration_cast<microseconds>(t2 - t1).count() / 1000.0);
    printf("Find boards took: %.3f ms/n", duration_cast<microseconds>(t4 - t3).count() / 1000.0);
    printf("Total took: %.3f ms/n", duration_cast<microseconds>(t2 - t1).count() / 1000.0 + duration_cast<microseconds>(t4 - t3).count() / 1000.0);
    cbdetect::plot_boards(img, corners, boards, params);
    for (const auto &board : boards)
    {
        std::cout << "num : " << board.num << std::endl;
        for (const auto &idx1 : board.idx)
        {
            for (const auto &index : idx1)
            {
                std::cout << index << " ";
            }
            std::cout << "\n";
        }
    }

    cv::waitKey(0);
}

void stereo(const char *left, const char *right)
{
    const char *left_w = "left";
    const char *right_w = "right";
    //
    cbdetect::Corner corners, rcorners;
    std::vector<cbdetect::Board> boards, rboards;
    cbdetect::Params params;
    params.corner_type = cbdetect::SaddlePoint;
    //
    cv::Mat limg = cv::imread(left, cv::IMREAD_COLOR);
    cv::Mat rimg = cv::imread(right, cv::IMREAD_COLOR);
    cbdetect::find_corners(limg, corners, params);
    cbdetect::find_corners(rimg, rcorners, params);

    cbdetect::plot_corners(rimg, rcorners, right_w);
    cbdetect::plot_corners(limg, corners, left_w);
    cv::waitKey();

    cbdetect::boards_from_corners(limg, corners, boards, params);
    cbdetect::boards_from_corners(rimg, rcorners, rboards, params);
    cbdetect::plot_boards(limg, corners, boards, params, "left");
    cbdetect::plot_boards(rimg, rcorners, rboards, params, "right");
    cv::waitKey();
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "error" << std::endl;
        return 0;
    }
    printf("chessboards...");
    if (argc == 2)
    {
        detect(argv[1], cbdetect::SaddlePoint);
    }
    else if (argc == 3)
    {
        stereo(argv[1], argv[2]);
    }
    return 0;
}
