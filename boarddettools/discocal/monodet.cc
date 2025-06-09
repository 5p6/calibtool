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
    argparse::ArgumentParser program("Circle Board Detection for Monocular Camera");

    program.add_argument("-i", "--input-dir")
        .help("Path of the input file dir")
        .required();

    program.add_argument("-o", "--output-dir")
        .help("Path of the output file dir")
        .default_value("./output");

    program.add_argument("--nx")
        .default_value(3)
        .scan<'i', int>()
        .help("Number of circles in the x-direction (columns)");

    program.add_argument("--ny")
        .default_value(4)
        .scan<'i', int>()
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

    fs::path output_dir(program.get<std::string>("--output-dir"));
    CSVWriter writer;

    fs::create_directories(output_dir);
    TargetDetector detector(program.get<int>("--nx"), program.get<int>("--ny"), program.get<bool>("--verbose"));

    for (const auto &entry : fs::directory_iterator(fs::directory_entry(path)))
    {
        // 检查后缀
        if (!iequals(entry.path().extension().string(), ".jpg"))
            continue;
        std::cout << entry.path().string() << std::endl;
        // 读图
        cv::Mat img = cv::imread(entry.path().string(), cv::IMREAD_GRAYSCALE);

        // 检测
        auto [ret, shapes] = detector.detect(img, "circle");
        if (!ret)
            continue;
        // 设置写入函数
        if (!writer.setwritefile((output_dir / entry.path().filename().replace_extension(".csv")).string()))
            throw std::runtime_error("could not open the file");
        // 写入
        writer << shapes;
    }
    return 0;
}