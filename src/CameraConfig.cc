#include "CameraConfig.h"

// 初始化
CameraType::CameraType(const std::string &param_file)
{
    read(param_file);
    // 初始化矫正映射图
    initremap();
}

// 读取参数
void CameraType::read(const std::string &param_file)
{
    cv::FileStorage file(param_file, cv::FileStorage::READ);

    // 数量类型
    cv::FileNode numNode = file["Camera_NumType"];
    numtype = CameraNumType::Monocular;
    if (numNode.string() == "Stereo")
    {
        numtype = CameraNumType::Stereo;
    }
    sensortype = CameraSensorType::Pinhole;
    // 传感器的类型
    cv::FileNode sensorNode = file["Camera_SensorType"];
    if (sensorNode.string() == "Fisheye")
    {
        sensortype = CameraSensorType::Fisheye;
    }
    else if (sensorNode.string() == "RGBD")
    {
        sensortype = CameraSensorType::RGBD;
    }

    // 单目参数
    K_l = file["K_l"].mat();
    D_l = file["D_l"].mat();

    // 图像尺寸
    ImageSize = cv::Size(file["width"].real(), file["height"].real());
    if (numtype == Stereo)
    {
        K_r = file["K_r"].mat();
        D_r = file["D_r"].mat();
        // 至少有 R,t 参数
        cv::FileNode Rnode = file["R"];
        cv::FileNode tnode = file["t"];
        if (!(Rnode.empty() || tnode.empty()))
        {
            std::cout << "read rotation matrix" << std::endl;
            R = Rnode.mat();
            std::cout << "read translation matrix" << std::endl;
            t = tnode.mat();
        }
        // 查看是否存在源参数
        cv::FileNode Plnode = file["P_l"];
        cv::FileNode Prnode = file["P_r"];
        cv::FileNode Rlnode = file["R_l"];
        cv::FileNode Rrnode = file["R_r"];
        cv::FileNode Qnode = file["Q"];
        // 查看是否为空
        if (Plnode.empty() || Prnode.empty() || Rlnode.empty() || Rrnode.empty() || Qnode.empty())
        {
            // 先判断  R , t
            CV_Assert(!(R.empty() || t.empty()) == true);
            cv::stereoRectify(
                K_l, D_l,
                K_r, D_r,
                ImageSize,
                R, t,
                R_l, R_r,
                P_l, P_r,
                Q, cv::CALIB_ZERO_DISPARITY);
        }
        else
        {
            // 全部都是存在的
            P_l = Plnode.mat();
            P_r = Prnode.mat();
            R_l = Rlnode.mat();
            R_r = Rrnode.mat();
            Q = Qnode.mat();
        }
    }
}

void CameraType::write(const std::string &param_file) const
{
    cv::FileStorage file(param_file, cv::FileStorage::WRITE);
    switch (sensortype)
    {
    case Pinhole:
        file.write("Camera_SensorType", std::string("Pinhole"));
        break;
    case Fisheye:
        file.write("Camera_SensorType", std::string("Fisheye"));
        break;
    case RGBD:
        file.write("Camera_SensorType", std::string("RGBD"));
        break;
    default:
        break;
    }

    switch (numtype)
    {
    case Monocular:
        file.write("Camera_NumType", std::string("Monocular"));
        file.write("K_l", K_l);
        file.write("D_l", D_l);
        break;
    case Stereo:
        file.write("Camera_NumType", std::string("Stereo"));
        file.write("K_l", K_l);
        file.write("D_l", D_l);
        file.write("K_r", K_r);
        file.write("D_r", D_r);
        // 第一种参数
        if (!(R.empty() || t.empty()))
        {
            file.write("R", R);
            file.write("t", t);
        }
        // 第二种参数
        if (!(P_l.empty() || P_r.empty() || R_l.empty() || R_r.empty() || Q.empty()))
        {
            file.write("P_l", P_l);
            file.write("P_r", P_r);
            file.write("R_l", R_l);
            file.write("R_r", R_r);
            file.write("Q", Q);
        }
        break;
    default:
        break;
    }
}

void CameraType::initremap()
{
    if (numtype == Monocular)
    {
        if (sensortype == Pinhole)
        {
            cv::initUndistortRectifyMap(K_l, D_l, cv::Mat::eye(cv::Size(3, 3), CV_32FC1), K_l, ImageSize, CV_32FC1, maplx, maply);
        }
        else if (sensortype == Fisheye)
        {
            cv::fisheye::initUndistortRectifyMap(K_l, D_l, cv::Mat::eye(cv::Size(3, 3), CV_32FC1), K_l, ImageSize, CV_32FC1, maplx, maply);
        }
    }
    else
    {
        if (sensortype == Pinhole)
        {
            cv::initUndistortRectifyMap(K_l, D_l, R_l, P_l, ImageSize, CV_32FC1, maplx, maply);
            cv::initUndistortRectifyMap(K_r, D_r, R_r, P_r, ImageSize, CV_32FC1, maprx, mapry);
        }
        else if (sensortype == Fisheye)
        {
            cv::fisheye::initUndistortRectifyMap(K_l, D_l, R_l, P_l, ImageSize, CV_32FC1, maplx, maply);
            cv::fisheye::initUndistortRectifyMap(K_r, D_r, R_r, P_r, ImageSize, CV_32FC1, maprx, mapry);
        }
    }
}

// 图像
cv::Mat CameraType::rectify(const cv::Mat &img)
{
    cv::Mat dst;
    cv::remap(img, dst, maplx, maply, cv::INTER_CUBIC);
    return dst;
}
std::pair<cv::Mat, cv::Mat> CameraType::rectify(const cv::Mat &left, const cv::Mat &right)
{
    cv::Mat dstl, dstr;
    // 矫正
    cv::remap(left, dstl, maplx, maply, cv::INTER_CUBIC);
    cv::remap(right, dstr, maprx, mapry, cv::INTER_CUBIC);
    // 返回
    return std::pair<cv::Mat, cv::Mat>(dstl, dstr);
}


void CameraType::Brief()
{
    std::cout << "left(rgb) Matrix : \n"
              << K_l << std::endl;
    std::cout << "left(rgb) distortion : \n"
              << D_l << std::endl;
    if (numtype == Stereo || sensortype == RGBD)
    {
        std::cout << "right(ir) Matrix : \n"
                  << K_r << std::endl;
        std::cout << "right(ir) distortion : \n"
                  << D_r << std::endl;
        if (!(R.empty() || t.empty()))
        {
            std::cout << "R : \n"
                      << R << std::endl;
            std::cout << "t : \n"
                      << t << std::endl;
        }
        if (!(P_l.empty() || P_r.empty() || R_l.empty() || R_r.empty()))
        {
            std::cout << "P_l : \n"
                      << P_l << std::endl;
            std::cout << "P_r : \n"
                      << P_r << std::endl;
            std::cout << "R_l : \n"
                      << R_l << std::endl;
            std::cout << "R_r : \n"
                      << R_r << std::endl;
        }
    }
}