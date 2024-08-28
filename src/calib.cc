#include <calib.h>
#include <libcbdetect/boards_from_cornres.h>
#include <libcbdetect/config.h>
#include <libcbdetect/find_corners.h>
#include <libcbdetect/plot_boards.h>
#include <libcbdetect/plot_corners.h>
#include <opencv2/core.hpp>

class CALIB_API OneshotCalibImpl : public OneshotCalib
{
public:
    // monocular
    OneshotCalibImpl(
        const std::string &img_path,
        CameraType::CameraSensorType sensorType,
        CameraType::CameraNumType numType,
        double _SquareSize);
    // stereo
    OneshotCalibImpl(
        const std::string &left_path,
        const std::string &right_path,
        CameraType::CameraSensorType sensorType,
        CameraType::CameraNumType numType,
        double _SquareSize);
    // read
    OneshotCalibImpl(const std::string &param_path);

    // detect --> optim --> write
    virtual void run();
    virtual void write(const std::string &param_file) const;

private:
    // 获取角点
    void detect();

    // 优化
    void Optimization();

    // generate object 3D points from the boards and corners
    /** @brief 从角点阵列和棋盘格中恢复三维点和二维角点
     * @param corners 所有的角点序列
     * @param boards 记录了每一个棋盘格和棋盘格角点对应的索引
     * @param imagepoints 转换后的二维图像角点
     * @param objpoints 转换后的三维点
     */
    void GetObj(const cbdetect::Corner &corners, const std::vector<cbdetect::Board> &boards, std::vector<cv::Point2d> &imagepoints, std::vector<cv::Point3d> &objpoints);

private:
    std::unique_ptr<CameraType> cam;
    cv::Mat image, right_image;

    // cbdetect param
    cbdetect::Corner corners, right_corners;
    std::vector<cbdetect::Board> boards, right_boards;

    // points from the board
    std::vector<cv::Point2d> left_points;
    std::vector<cv::Point2d> right_points;
    // object points
    std::vector<cv::Point3d> left_object_points;
    std::vector<cv::Point3d> right_object_points;
};
// mono
OneshotCalibImpl::OneshotCalibImpl(const std::string &img_path,
                                   CameraType::CameraSensorType sensorType,
                                   CameraType::CameraNumType numType,
                                   double _SquareSize)
{
    image = cv::imread(img_path);
    cam = std::make_unique<CameraType>(sensorType, numType, _SquareSize);
}
// stereo
OneshotCalibImpl::OneshotCalibImpl(
    const std::string &left_path,
    const std::string &right_path,
    CameraType::CameraSensorType sensorType,
    CameraType::CameraNumType numType,
    double _SquareSize)
{
    image = cv::imread(left_path);
    right_image = cv::imread(right_path);
    cam = std::make_unique<CameraType>(sensorType, numType, _SquareSize);
}
// read
OneshotCalibImpl::OneshotCalibImpl(const std::string &param_path)
{
    cam = std::make_unique<CameraType>(param_path);
}

// detect corners
void OneshotCalibImpl::detect()
{
    cbdetect::Params param;
    param.corner_type = cbdetect::CornerType::SaddlePoint;
    // get points
    cbdetect::find_corners(this->image, this->corners, param);
    // get board from the corners
    cbdetect::boards_from_corners(this->image, this->corners, this->boards, param);
    cam->ImageSize = image.size();
    // transform
    switch (cam->numtype)
    {
    case CameraType::CameraNumType::Monocular:
        GetObj(this->corners, this->boards, left_points, left_object_points);
        break;
    case CameraType::CameraNumType::Stereo:
        // right
        cbdetect::find_corners(this->right_image, this->right_corners, param);
        cbdetect::boards_from_corners(this->right_image, this->right_corners, this->right_boards, param);
        // 生成优化点
        GetObj(this->corners, this->boards, left_points, left_object_points);
        GetObj(this->right_corners, this->right_boards, right_points, right_object_points);
        break;
    default:
        break;
    }
}

//
void OneshotCalibImpl::Optimization()
{
    std::vector<cv::Mat> rvecs, tvecs;
    cv::Mat E, F;
    double res,lres,rres;
    int flag = cam->sensortype + cam->numtype << 1;
    switch (flag)
    {
    case 0:
        res = cv::calibrateCamera(
            left_object_points,
            left_points,
            cam->ImageSize,
            cam->K_l,
            cam->D_l,
            rvecs,
            tvecs);
        std::cout << "单目针孔重投影误差 : " << res << std::endl;
        break;
    case 1:
        res = cv::fisheye::calibrate(
            left_object_points,
            left_points,
            cam->ImageSize,
            cam->K_l,
            cam->D_l,
            rvecs,
            tvecs);
        std::cout << "单目鱼眼重投影误差 : " << res << std::endl;
        break;
    case 2:
        lres = cv::calibrateCamera(
            left_object_points,
            left_points,
            cam->ImageSize,
            cam->K_l,
            cam->D_l,
            rvecs,
            tvecs);
        rres = cv::calibrateCamera(
            right_object_points,
            right_points,
            cam->ImageSize,
            cam->K_r,
            cam->D_r,
            rvecs,
            tvecs);
        std::cout << "单目重投影误差 : \n"
                  << "左目 : " << lres << std::endl
                  << "右目 : " << rres << std::endl;
        res = cv::stereoCalibrate(
            left_object_points,
            left_points,
            right_points,
            cam->K_l, cam->D_l,
            cam->K_r, cam->D_r,
            cam->ImageSize,
            cam->R, cam->t,
            E, F);
        std::cout<<"双目重投影误差 : "<< res << std::endl;
        break;
    case 3:
        /*stere fisheye*/
        break;
    default:
        break;
    }
}

void OneshotCalibImpl::write(const std::string &param_file) const
{
    cam->write(param_file);
}

void OneshotCalibImpl::GetObj(const cbdetect::Corner &corners, const std::vector<cbdetect::Board> &boards, std::vector<cv::Point2d> &imagepoints, std::vector<cv::Point3d> &objpoints)
{
    bool flagcontinue = false;
    for (const auto &board : boards)
    {
        int countrow = 0;
        std::cout << "num : " << board.num << std::endl;
        for (const auto &indexarr : board.idx)
        {
            int countcol = 0;
            std::cout<<countrow<<": ";
            for (const auto &index : indexarr)
            {
                // 当索引为 <0 时,无效
                if (index < 0)
                {
                    continue;
                }
                // 添加图像点和对应的三维点
                imagepoints.emplace_back(corners.p[index]);
                objpoints.emplace_back(cv::Vec3d(countrow * cam->SquareSize, countcol * cam->SquareSize, 0.f));
                std::cout<<index<<" ";
                countcol++;
            }
            std::cout<<std::endl;
            countrow++;
        }
    }
}

// detect --> optim --> write
void OneshotCalibImpl::run()
{
    // 定位角点
    detect();
    // 相机优化
    // Optimization();
}

//-----------------------------------------------------------------
std::shared_ptr<OneshotCalib> OneshotCalib::create(const std::string &img_path,
                                                   CameraType::CameraSensorType sensorType,
                                                   CameraType::CameraNumType numType,
                                                   double _SquareSize)
{
    return std::make_shared<OneshotCalibImpl>(img_path, sensorType, numType, _SquareSize);
}
