#ifndef TARGETDETECTOR_H
#define TARGETDETECTOR_H

#include <iostream>
#include <string.h>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <unistd.h>
#include <stack>
#include <memory>
#include "chessboard/CircleGridFinder.hpp"
namespace camodocal
{
    class TargetDetector
    {
    public:
        TargetDetector(){}
        TargetDetector(int n_x, int n_y,bool draw = true);
        bool detect(const cv::Mat &img, std::vector<cv::Point2f>& corners);
    
    private:
        bool draw;
        int n_x, n_y;
        int size_threshold;
        float drawing_scale;
        double fullfill_threshold, eccentricity_threshold;
        std::vector<cv::Scalar> text_colors;

    private:
        bool detect_circles(cv::Mat img, std::vector<cv::Point2f> &target, bool debug = false);
        bool ellipse_test(const cv::Moments &moments);
        void sortTarget(std::vector<cv::Point2f> &source, std::vector<cv::Point2f> &dist);
    };
}
#endif