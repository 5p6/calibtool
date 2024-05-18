#include "calib.h"
#include "utility.h"

namespace calibration
{

    class CalibImpl : public Calib
    {
    public:
        CalibImpl(const std::string &_param_path)
        {
            read(_param_path);
        }
        CalibImpl(
            const CameraSensorType &_Camera_SensorType,
            const ChessboardType &_Chessboard_type,
            const std::string &_img_root_dir,
            const cv::Size &_board_size,
            const float _squre_size,
            const cv::Size &_radius_size,
            const cv::Size &_resize_times) : type_first(_Camera_SensorType), board_type(_Chessboard_type),
                                             board_size(_board_size), radius_size(_radius_size),
                                             square_size(_squre_size), resize_times(_resize_times)
        {
            type_second = Calib::Monocular;
            // 单目默认为左目
            left_paths = utility::getFilesFromPathWithoutSort(_img_root_dir);
        }

        CalibImpl(
            const CameraSensorType &_Camera_SensorType,
            const ChessboardType &_Chessboard_type,
            const std::string &_left_root_dir,
            const std::string &_right_root_dir,
            const cv::Size &_board_size,
            const float _squre_size,
            const cv::Size &_radius_size,
            const cv::Size &_resize_times) : type_first(_Camera_SensorType), board_type(_Chessboard_type),
                                             board_size(_board_size), radius_size(_radius_size),
                                             square_size(_squre_size), resize_times(_resize_times)
        {
            type_second = Calib::Stereo;
            // 双目
            left_paths = utility::getFilesFromPathWithoutSort(_left_root_dir);
            right_paths = utility::getFilesFromPathWithoutSort(_right_root_dir);
        }

        void GetRectifyParam()
        {
            int flag = type_first | type_second << 1;
            switch (flag)
            {
            case 0:
                std::cout << "the camera sensor type : Pinhole , the camera number type : Monocular" << std::endl;
                cv::initUndistortRectifyMap(K_l, D_l, cv::noArray(), K_l, image_size, CV_32FC1, mapxl, mapyl);
                break;
            case 1:
                std::cout << "the camera sensor type : Fisheye , the camera number type : Monocular" << std::endl;
                cv::fisheye::initUndistortRectifyMap(K_l, D_l, cv::noArray(), K_l, image_size, CV_32FC1, mapxl, mapyl);
                break;
            case 2:
                std::cout << "the camera sensor type : Pinhole , the camera number type : Stereo" << std::endl;
                cv::initUndistortRectifyMap(K_l, D_l, R_l, P_l, image_size, CV_32FC1, mapxl, mapyl);
                cv::initUndistortRectifyMap(K_r, D_r, R_r, P_r, image_size, CV_32FC1, mapxr, mapyr);
                break;
            case 3:
                std::cout << "the camera sensor type : Fisheye , the camera number type : Stereo" << std::endl;
                cv::fisheye::initUndistortRectifyMap(K_l, D_l, R_l, P_l, image_size, CV_32FC1, mapxl, mapyl);
                cv::fisheye::initUndistortRectifyMap(K_r, D_r, R_r, P_r, image_size, CV_32FC1, mapxr, mapyr);
                break;
            default:
                break;
            }
        }

    public:
        // virtual bool run() const;
        virtual bool read(const std::string &param_path);

        virtual bool write(const std::string &param_path) const;

        virtual void setflagshow(bool flag)
        {
            flag_show = flag;
        }
        virtual void setflagresize(bool flag)
        {
            flag_resize = flag;
        }

        virtual void run();

        virtual cv::Mat rectify(const cv::Mat &image);
        virtual std::pair<cv::Mat, cv::Mat> rectify(const cv::Mat &left_img, const cv::Mat &right_img);

    private:
        // 四种标定方法
        virtual bool monocalib();

        virtual bool monocalib_fisheye();

        virtual bool stereocalib();

        virtual bool stereocalib_fisheye();

    private:
        // 标定的参数
        Calib::CameraSensorType type_first;
        Calib::CameraNumType type_second;
        Calib::ChessboardType board_type = Calib::CornerGrid;
        cv::Size board_size, radius_size, resize_times;
        cv::Size image_size;
        float square_size;

        // 图像路径
        std::vector<std::string> left_paths, right_paths;

        // 标定结果的参数
        // 单目只有 K_l , D_l ,双目全都有
        cv::Mat R, t, Q;
        cv::Mat K_l, D_l, P_l, R_l;
        cv::Mat K_r, D_r, P_r, R_r;
        // 外参，单目默认为左目
        std::vector<cv::Mat> rvecs_l, tvecs_l;
        std::vector<cv::Mat> rvecs_r, tvecs_r;

        // 选项
        bool flag_show = false;
        bool flag_resize = false;

        // 矫正
        cv::Mat mapxl, mapyl;
        cv::Mat mapxr, mapyr;
    };

    // -------------------------------------------------------------------------------------------------------------------

    bool CalibImpl::read(const std::string &param_path)
    {
        cv::FileStorage file(param_path, cv::FileStorage::READ);
        if (!file.isOpened())
        {
            std::cout << "the parameters file does not open !" << std::endl;
            return false;
        }
        // 类型一查看
        type_first = Calib::Pinhole;
        if (file["Camera_SensorType"].string() == "Fisheye")
            type_first = Calib::Fisheye;

        // 参数导入
        file["K_l"] >> K_l;
        file["D_l"] >> D_l;
        // 类型二查看
        type_second = Calib::Monocular;
        image_size = cv::Size(file["width"], file["height"]);
        if (file["Camera_NumType"].string() == "Stereo")
        {
            type_second = Calib::Stereo;
            file["K_r"] >> K_r;
            file["D_r"] >> D_r;
            file["R_l"] >> R_l;
            file["R_r"] >> R_r;
            file["P_l"] >> P_l;
            file["P_r"] >> P_r;
            file["Q"] >> Q;
        }
        // 矫正畸变映射表的获取
        GetRectifyParam();
        return true;
    }

    bool CalibImpl::write(const std::string &param_path) const
    {
        cv::FileStorage file(param_path, cv::FileStorage::WRITE);
        // 查看是否标定了？
        if (K_l.empty())
        {
            std::cout << "there is not param here,please call the calib program !" << std::endl;
            return false;
        }

        if (type_first == Calib::Pinhole)
        {
            file << "Camera_SensorType"
                 << "Pinhole";
        }
        else
        {
            file << "Camera_SensorType"
                 << "Fisheye";
        }

        if (type_second == Calib::Monocular)
        {
            file << "Camera_NumType"
                 << "Monocular"
                 << "K_l" << K_l
                 << "D_l" << D_l;
        }
        else
        {
            file << "Camera_NumType"
                 << "Stereo"
                 << "K_l" << K_l
                 << "D_l" << D_l
                 << "K_r" << K_r
                 << "D_r" << D_r
                 << "R_l" << R_l
                 << "R_r" << R_r
                 << "P_l" << P_l
                 << "P_r" << P_r
                 << "Q" << Q;
        }
        file << "height" << image_size.height
             << "width" << image_size.width;
        return true;
    }

    void CalibImpl::run()
    {
        int flag = type_first | type_second << 1;
        switch (flag)
        {
        case 0:
            std::cout << "the camera sensor type : Pinhole , the camera number type : Monocular " << std::endl;
            this->monocalib();
            break;
        case 1:
            std::cout << "the camera sensor type : Fisheye , the camera number type : Monocular " << std::endl;
            this->monocalib_fisheye();
            break;
        case 2:
            std::cout << "the camera sensor type : Pinhole , the camera number type : Stereo " << std::endl;
            this->stereocalib();
            break;
        case 3:
            std::cout << "the camera sensor type : Fisheye , the camera number type : Stereo " << std::endl;
            this->stereocalib_fisheye();
            break;

        default:
            break;
        }
    }

    bool CalibImpl::monocalib()
    {
        // board size
        int rows = board_size.height;
        int cols = board_size.width;
        int board_n = rows * cols;
        // 左目的图像点和对应的三维点
        std::vector<std::vector<cv::Point2f>> left_points;
        std::vector<cv::Point2f> left_corners;
        // 图像
        cv::Mat left_img;
        cv::Mat left_gray;
        // 一些设置
        bool left_ret;
        for (auto left_name : left_paths)
        {

            left_img = cv::imread(left_name);
            if (flag_resize)
            {
                cv::resize(left_img, left_img, cv::Size(), resize_times.height, resize_times.width);
            }
            cv::cvtColor(left_img, left_gray, cv::COLOR_BGR2GRAY);
            if (board_type == ChessboardType::CircleGrid)
            {
                left_ret = cv::findCirclesGrid(left_gray, board_size, left_corners, cv::CALIB_CB_ADAPTIVE_THRESH);
            }
            else
            {
                left_ret = cv::findChessboardCorners(left_gray, board_size, left_corners, cv::CALIB_CB_ADAPTIVE_THRESH);
            }

            if (!left_ret)
            {
                std::cout << "find the corner default" << std::endl;
                continue;
            }
            std::cout << "find corners and the file : " << left_name  << std::endl;
            // 亚角点 圆标定板不找棋盘格
            if (board_type == ChessboardType::CornerGrid)
            {
                cv::cornerSubPix(left_gray, left_corners, radius_size, cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::Type::EPS + cv::TermCriteria::Type::MAX_ITER, 50, 1e-6));
            }
            // 绘画
            cv::drawChessboardCorners(left_img, board_size, left_corners, left_ret);
            cv::namedWindow("left", cv::WINDOW_NORMAL);
            cv::imshow("left", left_img);
            if (flag_show)
            {
                while (1)
                {
                    if (cv::waitKey(1) == 27)
                        break;
                }
            }
            else
            {
                cv::waitKey(500);
            }
            // push back the left image points
            left_points.push_back(left_corners);
        }
        // destroy all windows
        cv::destroyAllWindows();
        // get the image size
        image_size = left_img.size();
        // set the point
        std::vector<std::vector<cv::Point3f>> objpoints;
        objpoints.reserve(left_points.size());
        std::vector<cv::Point3f> points;
        points.reserve(board_n);
        // 基于范围的循环,可以保持数量的一致
        for (auto corners : left_points)
        {
            points.clear();
            for (int i = 0; i < board_n; i++)
            {
                int x = i % cols;
                cv::Point3f point(x * square_size, square_size * (i - x) / cols, 0);
                points.emplace_back(point);
            }
            objpoints.emplace_back(points);
        }

        std::cout << "beging to calibrate !" << std::endl;
        // set the optimization flag
        int flags = 0;
        // flags |= cv::CALIB_FIX_K1;
        // flags |= cv::CALIB_FIX_INTRINSIC;
        // flags |= cv::CALIB_RATIONAL_MODEL + cv::CALIB_TILTED_MODEL;
        // 标定
        double rms = cv::calibrateCamera(
            objpoints,
            left_points,
            image_size,
            K_l,
            D_l,
            rvecs_l,
            tvecs_l,
            flags,
            cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 500, 0));
        std::cout << "残差 : " << rms << std::endl;
        std::cout << "内参矩阵 K_l: \n"
                  << K_l << std::endl;
        std::cout << "畸变系数 D_l : \n"
                  << D_l << std::endl;
        return true;
    }

    bool CalibImpl::monocalib_fisheye()
    {
        int rows = board_size.height;
        int cols = board_size.width;
        int board_n = rows * cols;
        // 点集
        std::vector<std::vector<cv::Point2f>> left_points;
        std::vector<cv::Point2f> left_corners;
        // 数据
        cv::Mat left;
        cv::Mat left_gray;
        cv::Mat picure;
        bool ret = false;
        for (auto left_name : left_paths)
        {
            left = cv::imread(left_name);
            if (flag_resize)
            {
                cv::resize(left, left, cv::Size(), resize_times.height, resize_times.width);
            }
            cv::cvtColor(left, left_gray, cv::COLOR_BGR2GRAY);
            // 标定板类型不同导致查找标定点算法不同
            if (board_type == ChessboardType::CircleGrid)
            {
                ret = cv::findCirclesGrid(left_gray, board_size, left_corners,
                                          cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE);
            }
            else
            {
                ret = cv::findChessboardCorners(left_gray, board_size, left_corners,
                                                cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE);
            }
            if (!ret)
            {
                std::cout << "角点未找到,图像的路径为 :" << left_name << std::endl;
                continue;
            }
            std::cout << "角点查找成功,图像路径为 : " << left_name << std::endl;
            if (board_type == ChessboardType::CornerGrid)
            {
                cv::cornerSubPix(left_gray, left_corners, radius_size, cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::Type::EPS + cv::TermCriteria::Type::MAX_ITER, 100, 1e-6));
            }
            cv::drawChessboardCorners(left, board_size, left_corners, ret);
            cv::namedWindow("img", cv::WINDOW_NORMAL);
            cv::imshow("img", left);
            if (flag_show)
            {
                while (1)
                {
                    if (cv::waitKey(1) == 27)
                        break;
                }
            }
            else
            {
                cv::waitKey(500);
            }
            left_points.push_back(left_corners);
        }
        cv::destroyAllWindows();

        // ־
        int flag = 0;
        // flag += cv::fisheye::CALIB_RECOMPUTE_EXTRINSIC;
        flag += cv::fisheye::CALIB_FIX_SKEW;
        // flag += cv::fisheye::CALIB_FIX_INTRINSIC;
        // flag += cv::fisheye::CALIB_FIX_PRINCIPAL_POINT;
        flag += cv::fisheye::CALIB_RECOMPUTE_EXTRINSIC;
        //
        image_size = left.size();
        // 世界点
        std::vector<std::vector<cv::Point3f>> objpoints;
        objpoints.reserve(left_points.size());
        std::vector<cv::Point3f> points;
        points.reserve(board_n + 1);
        for (auto corner : left_points)
        {
            points.clear();
            for (int i = 0; i < board_n; i++)
            {
                int x = i % cols;
                cv::Point3f point(x * square_size, square_size * (i - x) / cols, 0);
                points.push_back(point);
            }
            objpoints.push_back(points);
        }

        // 标定
        double rms = cv::fisheye::calibrate(
            objpoints,
            left_points,
            image_size,
            K_l,
            D_l,
            rvecs_l,
            tvecs_l,
            flag); // رļ
        std::cout << "rms :" << rms << std::endl;
        std::cout << "内参矩阵 K : \n"
                  << K_l << std::endl;
        std::cout << "畸变系数 D : \n"
                  << D_l << std::endl;
        return true;
    }

    bool CalibImpl::stereocalib()
    {
        if (right_paths.empty())
        {
            std::cout << "右目图像数量是空的" << std::endl;
            return false;
        }
        int rows = board_size.height;
        int cols = board_size.width;
        int board_n = rows * cols;
        // 世界点
        std::vector<std::vector<cv::Point3f>> objpoints;
        std::vector<cv::Point3f> points;
        // 点集
        std::vector<std::vector<cv::Point2f>> left_points;
        std::vector<std::vector<cv::Point2f>> right_points;
        std::vector<cv::Point2f> left_corners;
        std::vector<cv::Point2f> right_corners;
        // 预设图像
        cv::Mat left_img, right_img;
        cv::Mat left_gray, right_gray;
        // Ƿнǵ
        bool left_ret, right_ret;
        for (int i = 0; i < left_paths.size(); i++)
        {
            // ͼƬ
            left_img = cv::imread(left_paths[i]);
            right_img = cv::imread(right_paths[i]);
            if (flag_resize)
            {
                cv::resize(left_img, left_img, cv::Size(), resize_times.height, resize_times.width);
                cv::resize(right_img, right_img, cv::Size(), resize_times.height, resize_times.width);
            }
            cv::cvtColor(left_img, left_gray, cv::COLOR_BGR2GRAY);
            cv::cvtColor(right_img, right_gray, cv::COLOR_BGR2GRAY);
            // 角点查找
            if (board_type == ChessboardType::CornerGrid)
            {
                left_ret = cv::findChessboardCorners(left_gray, board_size, left_corners, cv::CALIB_CB_ADAPTIVE_THRESH);
                right_ret = cv::findChessboardCorners(right_gray, board_size, right_corners, cv::CALIB_CB_ADAPTIVE_THRESH);
            }
            else
            {
                left_ret = cv::findCirclesGrid(left_gray, board_size, left_corners, cv::CALIB_CB_ADAPTIVE_THRESH);
                right_ret = cv::findCirclesGrid(right_gray, board_size, right_corners, cv::CALIB_CB_ADAPTIVE_THRESH);
            }
            if (!(left_ret & right_ret))
            {
                std::cout << "角点未找到,图像序列 :" << left_paths[i] <<" && " << right_paths[i] << std::endl;
                continue;
            }
            std::cout << "找到了角点,图像名称 :" << left_paths[i] <<" && " <<right_paths[i] << std::endl;
            if (board_type == ChessboardType::CornerGrid)
            {
                // 亚角点
                cv::cornerSubPix(left_gray, left_corners, radius_size, cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::Type::EPS + cv::TermCriteria::Type::MAX_ITER, 50, 1e-6));
                cv::cornerSubPix(right_gray, right_corners, radius_size, cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::Type::EPS + cv::TermCriteria::Type::MAX_ITER, 50, 1e-6));
            }
            // 画角点
            cv::drawChessboardCorners(left_img, board_size, left_corners, left_ret);
            cv::drawChessboardCorners(right_img, board_size, right_corners, right_ret);

            // 显示
            cv::imshow("left", left_img);
            cv::imshow("right", right_img);
            if (flag_show)
            {
                while (1)
                {
                    if (cv::waitKey(1) == 27)
                        break;
                }
            }
            else
            {
                cv::waitKey(500);
            }
            // 角点推入
            left_points.push_back(left_corners);
            right_points.push_back(right_corners);
        }
        // 清空窗口
        cv::destroyAllWindows();
        // 获取尺寸
        image_size = left_img.size();
        //
        int flags = 0;
        // flags |= cv::CALIB_FIX_K1;
        // flags |= cv::CALIB_FIX_INTRINSIC;
        // flags |= cv::CALIB_RATIONAL_MODEL + cv::CALIB_TILTED_MODEL;
        //
        for (auto corners : left_points)
        {
            points.clear();
            for (int i = 0; i < board_n; i++)
            {
                int x = i % cols;
                cv::Point3f point(x * square_size, square_size * (i - x) / cols, 0);
                points.push_back(point);
            }
            objpoints.push_back(points);
        }
        std::cout << "开始标定" << std::endl;
        // 各自标定
        double rms1 = cv::calibrateCamera(objpoints, left_points, image_size, K_l, D_l, rvecs_l, tvecs_l, 0, cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 60, 0));
        double rms2 = cv::calibrateCamera(objpoints, right_points, image_size, K_r, D_r, rvecs_r, tvecs_r, 0, cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 60, 0));
        std::cout << "左相机残差 :" << rms1 << std::endl;
        std::cout << "左目 K_l : \n"
                  << K_l << "\n"
                  << "D_l : \n"
                  << D_l << std::endl;
        ;
        std::cout << "右相机残差 : " << rms2 << std::endl;
        std::cout << "右目 K_r : \n"
                  << K_r << "\n"
                  << "D_r:\n"
                  << D_r << std::endl;
        ;

        // 旋转矩阵
        cv::Mat R, T;
        // 本征矩阵和基本矩阵
        cv::Mat E, F;
        // RTEE
        double rms = cv::stereoCalibrate(
            objpoints,
            left_points,
            right_points,
            K_l,
            D_l,
            K_r,
            D_r,
            image_size,
            R,
            T,
            E,
            F,
            flags,
            cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 60, 1e-8));
        std::cout << "双目残差:" << rms << std::endl;
        std::cout << "左目内参 K_l :\n"
                  << K_l << "\n"
                  << "畸变系数 D : \n"
                  << D_l << std::endl;
        ;
        std::cout << "右目内参 K_r :\n"
                  << K_r << "\n"
                  << "畸变系数 D_r : \n"
                  << D_r << std::endl;
        ;
        std::cout << "R : \n"
                  << R << std::endl;
        std::cout << "T : \n "
                  << T << std::endl;

        // 矫正
        cv::stereoRectify(
            K_l,
            D_l,
            K_r,
            D_r,
            image_size,
            R,
            T,
            R_l,
            R_r,
            P_l,
            P_r,
            Q);
        return true;
    }

    bool CalibImpl::stereocalib_fisheye()
    {
        int rows = board_size.height;
        int cols = board_size.width;
        int board_n = rows * cols;
        // 图像路径
        std::vector<std::string> left_path;
        std::vector<std::string> right_path;
        // ͼǵ
        std::vector<std::vector<cv::Point2f>> left_points;
        std::vector<std::vector<cv::Point2f>> right_points;
        std::vector<cv::Point2f> left_corners;
        std::vector<cv::Point2f> right_corners;
        // 图像
        cv::Mat left_img, right_img;
        cv::Mat left_gray, right_gray;
        // Ƿнǵ
        bool left_ret, right_ret;
        // Ƿʾ
        for (int i = 0; i < left_paths.size(); i++)
        {
            left_img = cv::imread(left_paths[i]);
            right_img = cv::imread(right_paths[i]);
            if (flag_resize)
            {
                cv::resize(left_img, left_img, cv::Size(), resize_times.height, resize_times.width);
                cv::resize(right_img, right_img, cv::Size(), resize_times.height, resize_times.width);
            }
            cv::cvtColor(left_img, left_gray, cv::COLOR_BGR2GRAY);
            cv::cvtColor(right_img, right_gray, cv::COLOR_BGR2GRAY);
            // 角点查找
            if (board_type == ChessboardType::CornerGrid)
            {
                left_ret = cv::findChessboardCorners(left_gray, board_size, left_corners, cv::CALIB_CB_ADAPTIVE_THRESH);
                right_ret = cv::findChessboardCorners(right_gray, board_size, right_corners, cv::CALIB_CB_ADAPTIVE_THRESH);
            }
            else
            {
                left_ret = cv::findCirclesGrid(left_gray, board_size, left_corners, cv::CALIB_CB_ADAPTIVE_THRESH);
                right_ret = cv::findCirclesGrid(right_gray, board_size, right_corners, cv::CALIB_CB_ADAPTIVE_THRESH);
            }
            //
            if (!(left_ret & right_ret))
            {
                std::cout << "角点查找失败,图像路径 : " << left_paths[i] <<" && " <<right_paths[i] << std::endl;
                continue;
            }
            std::cout << "角点查找成功,图像路径 : " << left_paths[i] <<" && " <<right_paths[i] << std::endl;
            // 亚角点查找
            if (board_type == ChessboardType::CornerGrid)
            {
                // 亚角点
                cv::cornerSubPix(left_gray, left_corners, radius_size, cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::Type::EPS + cv::TermCriteria::Type::MAX_ITER, 50, 1e-6));
                cv::cornerSubPix(right_gray, right_corners, radius_size, cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::Type::EPS + cv::TermCriteria::Type::MAX_ITER, 50, 1e-6));
            }
            cv::drawChessboardCorners(left_img, board_size, left_corners, left_ret);
            cv::drawChessboardCorners(right_img, board_size, right_corners, right_ret);
            cv::imshow("left", left_img);
            cv::imshow("right", right_img);
            if (flag_show)
            {
                while (1)
                {
                    if (cv::waitKey(1) == 27)
                        break;
                }
            }
            else
            {
                cv::waitKey(500);
            }
            // 角点推入
            left_points.push_back(left_corners);
            right_points.push_back(right_corners);
        }
        cv::destroyAllWindows();
        //
        int flag = 0;
        flag += cv::fisheye::CALIB_RECOMPUTE_EXTRINSIC;
        // flag += cv::fisheye::CALIB_FIX_SKEW;
        // flag += cv::fisheye::CALIB_FIX_INTRINSIC;
        // flag += cv::fisheye::CALIB_FIX_PRINCIPAL_POINT;
        // flag += cv::fisheye::CALIB_FIX_K4;
        // 角点
        image_size = left_img.size();
        // 世界点
        std::vector<std::vector<cv::Point3f>> objpoints;
        objpoints.reserve(left_points.size() + 1);
        std::vector<cv::Point3f> points;
        points.reserve(board_n + 1);
        for (auto corners : left_points)
        {
            points.clear();
            for (int i = 0; i < board_n; i++)
            {
                float x = static_cast<float>(i % cols);
                cv::Point3f point(x, (float)(i - x) / cols, 0);
                points.push_back(point);
            }
            objpoints.push_back(points);
        }

        // 各自标定
        double rms1 = cv::fisheye::calibrate(objpoints, left_points, image_size, K_l, D_l, rvecs_l, tvecs_l, flag, cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 100, 1e-5));
        double rms2 = cv::fisheye::calibrate(objpoints, right_points, image_size, K_r, D_r, rvecs_r, tvecs_r, flag, cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 100, 1e-5));
        std::cout << "左相机残差 :" << rms1 << std::endl;
        std::cout << "左目 K_l: \n"
                  << K_l << "\n"
                  << "D_l : \n"
                  << D_l << std::endl;
        ;
        std::cout << "右相机残差 : " << rms2 << std::endl;
        std::cout << "右目 K_r: \n"
                  << K_r << "\n"
                  << "D_r : \n"
                  << D_r << std::endl;
        ;

        // 联合标定
        cv::Mat R, T;
        std::cout << "开始双目融合标定" << std::endl;
        // R,T
        double rms = cv::fisheye::stereoCalibrate(
            objpoints,
            left_points,
            right_points,
            K_l,
            D_l,
            K_r,
            D_r,
            image_size,
            R,
            T);
        std::cout << "双目残差:" << rms << std::endl;
        std::cout << "左目内参矩阵 K_l :\n"
                  << K_l << "\n"
                  << "畸变参数 D_l : \n"
                  << D_l << std::endl;
        ;
        std::cout << "右目内参矩阵 K_r :\n"
                  << K_r << "\n"
                  << "畸变参数 D_r :\n"
                  << D_r << std::endl;
        ;
        std::cout << "R : \n"
                  << R << std::endl;
        std::cout << "T : \n "
                  << T << std::endl;
        cv::fisheye::stereoRectify(
            K_l,
            D_l,
            K_r,
            D_r,
            image_size,
            R, T,
            R_l, R_r, P_l, P_r, Q,
            0);
        return true;
    }

    std::shared_ptr<Calib> Calib::create(const std::string &_param_path)
    {
        return std::make_shared<CalibImpl>(_param_path);
    }
    std::shared_ptr<Calib> Calib::create(
        const CameraSensorType &_Camera_SensorType,
        const ChessboardType &_Chessboard_type,
        const std::string &_img_root_dir,
        const cv::Size &_board_size,
        const float _squre_size,
        const cv::Size &_radius_size,
        const cv::Size &_resize_times)
    {
        return std::make_shared<CalibImpl>(_Camera_SensorType, _Chessboard_type, _img_root_dir, _board_size, _squre_size, _radius_size, _resize_times);
    }

    std::shared_ptr<Calib> Calib::create(
        const CameraSensorType &_Camera_SensorType,
        const ChessboardType &_Chessboard_type,
        const std::string &_left_root_dir,
        const std::string &_right_root_dir,
        const cv::Size &_board_size,
        const float _squre_size,
        const cv::Size &_radius_size,
        const cv::Size &_resize_times)
    {
        return std::make_shared<CalibImpl>(_Camera_SensorType, _Chessboard_type, _left_root_dir, _right_root_dir, _board_size, _squre_size, _radius_size, _resize_times);
    }

    cv::Mat CalibImpl::rectify(const cv::Mat &image)
    {
        // 异常处理机制
        CV_Assert(image_size == image.size());
        cv::Mat rectify_image;
        cv::remap(image, rectify_image, mapxl, mapyl, cv::INTER_CUBIC, cv::BORDER_TRANSPARENT);
        return rectify_image;
    }
    std::pair<cv::Mat, cv::Mat> CalibImpl::rectify(const cv::Mat &left_img, const cv::Mat &right_img)
    {
        // 异常处理机制
        CV_Assert(image_size == left_img.size());
        cv::Mat rectify_left, rectify_right;
        cv::remap(left_img, rectify_left, mapxl, mapyl, cv::INTER_CUBIC, cv::BORDER_TRANSPARENT);
        cv::remap(right_img, rectify_right, mapxl, mapyl, cv::INTER_CUBIC, cv::BORDER_TRANSPARENT);
        return std::make_pair(rectify_left, rectify_right);
    }
}
