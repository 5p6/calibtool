#ifndef __UTILS_H__
#define __UTILS_H__

#include <filesystem>
#include <iostream>
#include <string_view>
#include <fstream>
#include <vector>
#include <iomanip>
struct Shape
{
    double x, y;
    double m00, m10, m01, m20, m11, m02;
    double Kxx, Kxy, Kyy;
    int_least32_t n;
    int ks;
    int bs;
    Shape(double _cx, double _cy, double _area)
    {
        x = _cx;
        y = _cy;
        m00 = _area;
        m10 = m00 * x;
        m01 = m00 * y;
        m20 = _area / (4 * M_PI);
        m11 = 0;
        m02 = _area / (4 * M_PI);
        Kxx = 0;
        Kxy = 0;
        Kyy = 0;
        n = 0;
    }
    Shape(int _n, double _m00, double _m10, double _m01, double _m20, double _m11, double _m02, double _Kxx = 0, double _Kxy = 0, double _Kyy = 0)
    {
        n = _n;
        m00 = _m00;
        m10 = _m10;
        m01 = _m01;
        m20 = _m20;
        m11 = _m11;
        m02 = _m02;
        Kxx = _Kxx;
        Kxy = _Kxy;
        Kyy = _Kyy;

        x = m10 / m00;
        y = m01 / m00;
    }
    double uncertainty()
    {
        double det = Kxx * Kyy - Kxy * Kxy;
        return 0.5 * (log(2 * M_PI * det) + 1.0);
        // return Kxx+Kxy;
        // return Kxx*Kyy-Kxy*Kxy;
    }
    std::string to_string()
    {
        std::string str = std::to_string(x) + "\t" + std::to_string(y) + "\t" + std::to_string(Kxx) + "\t" + std::to_string(Kxy) + "\t" + std::to_string(Kyy);
        return str;
    }
};

class CSVWriter
{
public:
    CSVWriter() = default;
    bool setwritefile(const std::string& filename){
        outFile.open(filename);
        return outFile.is_open();
    }
    ~CSVWriter()
    {
    }

    CSVWriter &operator<<(std::vector<Shape> &shapes)
    {
        // 表头
        outFile << "image_x image_y" << std::endl;
        outFile << std::fixed << std::setprecision(6);
        // 写入数据
        for (const auto& shape : shapes)
        {
            outFile << shape.x << " " << shape.y << std::endl;
        }
        outFile.close();
        return *this;
    }

    CSVWriter &operator<<(std::vector<std::tuple<float, float, float>> &points)
    {
        // 表头
        outFile << "world_x world_y world_z" << std::endl;
        outFile << std::fixed << std::setprecision(6);
        // 写入数据
        for (auto& point : points)
        {
            outFile << std::get<0>(point) << " " << std::get<1>(point) << " " << std::get<2>(point) << std::endl;
        }
        outFile.close();
        return *this;
    }
private:
    std::ofstream outFile;
};

#endif