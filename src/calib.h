#ifndef CALIB_H
#define CALIB_H

#include <iostream>
#include <string>
#include <vector>
#include "config.h"
#include "CameraConfig.h"

class CALIB_API OneshotCalib
{

public:
    virtual void run() = 0;
    virtual void write(const std::string& param_file) const = 0;
    // monocular
    static std::shared_ptr<OneshotCalib> create(const std::string &img_path,
                                         CameraType::CameraSensorType sensorType,
                                         CameraType::CameraNumType numType,
                                         double _SquareSize
                                         );
    // stereo
    static std::shared_ptr<OneshotCalib> create(const std::string &left_path,
                                         const std::string &right_path,
                                         CameraType::CameraSensorType sensorType,
                                         CameraType::CameraNumType numType,
                                         double _SquareSize
                                         );
    // read
    static std::shared_ptr<OneshotCalib> create(const std::string &param_path);
};

#endif