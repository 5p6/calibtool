#include "CTargetDetector.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include "argparse.hpp"
namespace fs = std::filesystem;
bool iequals(const std::string &a, const std::string &b)
{
    return std::equal(a.begin(), a.end(),
                      b.begin(), b.end(),
                      [](char a_char, char b_char)
                      {
                          return std::tolower(static_cast<unsigned char>(a_char)) ==
                                 std::tolower(static_cast<unsigned char>(b_char));
                      });
}

int main(int argc, char **argv)
{
    argparse::ArgumentParser program("Circle Board Detection for Stereo Camera");

    program.add_argument("-i", "--input-dir")
        .help("Input file")
        .required();

    program.add_argument("-o", "--output-dir")
        .help("Output file")
        .default_value("./output");

    program.add_argument("--nx")
        .default_value(3)
        .scan<'i',int>()
        .help("Number of circles in the x-direction (columns)");

    program.add_argument("--ny")
        .default_value(4)
        .scan<'i',int>()
        .help("Number of circles in the y-direction (rows)");

    program.add_argument("--verbose")
        .help("Enable verbose mode")
        .default_value(false)
        .implicit_value(true);

    try
    {
        program.parse_args(argc, argv);
    }
    catch (const std::runtime_error &err)
    {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    fs::path path(program.get<std::string>("--input-dir"));
    fs::path left_path = path / "left";
    fs::path right_path = path / "right";

    fs::path output_dir(program.get<std::string>("--output-dir"));
    fs::path left_output = output_dir / "left_corners";
    fs::path right_output = output_dir / "right_corners";
    CSVWriter leftwriter;
    CSVWriter rightwriter;

    fs::create_directories(left_output);
    fs::create_directories(right_output);
    TargetDetector detector(program.get<int>("--nx"), program.get<int>("--ny"), program.get<bool>("--verbose"));

    for (const auto &entry : fs::directory_iterator(fs::directory_entry(left_path)))
    {
        // 检查后缀
        if (!iequals(entry.path().extension().string(), ".jpg"))
            continue;
        // 检查同名右目
        if (!fs::exists(right_path / entry.path().filename()))
        {
            std::cout << "right file " << right_path / entry.path().filename() << " do not exist" << std::endl;
            continue;
        }
        std::cout << entry.path().string() << std::endl;
        // 读图
        cv::Mat leftimg = cv::imread(entry.path().string(), cv::IMREAD_GRAYSCALE);
        cv::Mat rightimg = cv::imread((right_path / entry.path().filename()).string(), cv::IMREAD_GRAYSCALE);

        // 检测
        auto [ret, leftshapes, rightshapes] = detector.DetectStereoCircle(leftimg, rightimg);
        if (!ret)
            continue;
        // 设置写入函数
        if (!leftwriter.setwritefile((left_output / entry.path().filename().replace_extension(".csv")).string()))
            throw std::runtime_error("could not open the file");
        if (!rightwriter.setwritefile((right_output / entry.path().filename().replace_extension(".csv")).string()))
            throw std::runtime_error("could not open the file");
        // 写入
        leftwriter << leftshapes;
        rightwriter << rightshapes;
    }
    return 0;
}