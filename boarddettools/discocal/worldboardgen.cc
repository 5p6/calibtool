#include "CTargetDetector.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <iomanip>
#include "argparse.hpp"
namespace fs = std::filesystem;

int main(int argc, char *argv[])
{
    argparse::ArgumentParser program("Circle Board Detection for Stereo Camera");
    program.add_argument("-o", "--output-dir")
        .help("Output file")
        .default_value("./output");

    program.add_argument("--nx")
        .default_value(3)
        .scan<'i', int>()
        .help("Number of circles in the x-direction (columns)");

    program.add_argument("--ny")
        .default_value(4)
        .scan<'i', int>()
        .help("Number of circles in the y-direction (rows)");
    program.add_argument("--square-size")
        .default_value(0.2f)
        .scan<'f', float>()
        .help("the distance of point (m)");
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

    fs::path output_dir(program.get<std::string>("--output-dir"));
    fs::create_directories(output_dir);

    CSVWriter writer;
    if (!writer.setwritefile((output_dir / "world_coordinates.csv")))
        throw std::runtime_error("could not open the file");

    int nx = program.get<int>("--nx");
    int ny = program.get<int>("--ny");
    float square = program.get<float>("--square-size");
    std::vector<std::tuple<float, float, float>> points;
    for (int i = 0; i < nx * ny; i++)
    {
        float x = i % nx;
        float y = (i - i % nx) / nx;
        points.emplace_back(std::make_tuple(x * square, y * square, 0.f));
    }

    writer << points;
    return 0;
}