#ifndef CALIB_h
#define CALIB_h

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>



namespace calibration{
    /** @brief 标定类别支持四种模式 [Pinhole,Fisheye] / [Monocular,Stereo] 各选一个
     * 
    */
    class Calib {
        public:
        /** @brief 三个个标志
        */
        enum CameraSensorType{
            Pinhole = 0,
            Fisheye = 1
        };
        enum CameraNumType {
            Monocular = 0,
            Stereo = 1
        };
        enum ChessboardType{
            CircleGrid = 0,
            CornerGrid
        };


        public:

        /** @brief 单目标定的生成
         * @param param_path 参数文件路径
        */
        static std::shared_ptr<Calib> create(const std::string& _param_path);


        /** @brief 单目标定的生成
         * @param Camera_SensorType 相机类型
         * @param img_root_dir 图像根目录路径
         * @param board_size 棋盘格角点尺寸
         * @param radius_size 亚角点查找的半径
         * @param square_size 棋盘格小格子的尺寸(mm)
         * @param resize_times 重定尺寸的选项,默认为不修改
        */
        static std::shared_ptr<Calib> create(
            const CameraSensorType& _Camera_SensorType,
            const ChessboardType& _Chessboard_type,
            const std::string& _img_root_dir,
            const cv::Size& _board_size,
            const float _squre_size,
            const cv::Size& _radius_size = cv::Size(3,3),
            const cv::Size& _resize_times = cv::Size(1,1)
            );

        /** @brief 双目标定的生成
         * @param Camera_SensorType 相机类型
         * @param left_root_dir 左摄像机图像根目录路径
         * @param right_root_dir 右摄像机图像的根目录路径
         * @param board_size 棋盘格角点尺寸
         * @param radius_size 亚角点查找的半径
         * @param square_size 棋盘格小格子的尺寸(mm)
         * @param resize_times 重定尺寸的选项,默认为不修改
        */
       static std::shared_ptr<Calib> create(
            const CameraSensorType& _Camera_SensorType,
            const ChessboardType& _Chessboard_type,
            const std::string& _left_root_dir,
            const std::string& _right_root_dir,
            const cv::Size& _board_size,
            const float _squre_size,
            const cv::Size& _radius_size = cv::Size(3,3),
            const cv::Size& _resize_times = cv::Size(1,1)
            );

        
        /** @brief 执行程序调用
         * 
        */
        virtual void run() = 0;

        /** @brief 矫正,包含单目和双目的重载
         * 
         * 
        */
       virtual cv::Mat rectify(const cv::Mat& image) = 0;
       virtual std::pair<cv::Mat,cv::Mat> rectify(const cv::Mat& left_img,const cv::Mat& right_img) = 0;

        /** @brief 展示标志
         * 
        */
        virtual void setflagshow(bool flag) = 0;
        /** @brief 重定尺寸标志
         * 
        */
       virtual void setflagresize(bool flag) = 0;
        /** @brief 读取参数
         * 
        */
       virtual bool read(const std::string& param_path) = 0;

        /** @brief 保存参数
        */
        virtual bool write(const std::string& param_path) const = 0;


        protected:


        /** @brief 单目针孔相机标定程序 
        */
        virtual bool monocalib() = 0;

        /** @brief 单目鱼眼相机标定程序
         * 
         * 
        */
        virtual bool monocalib_fisheye() = 0;

        /** @brief 双目针孔相机标定程序
         * 
        */
        virtual bool stereocalib() = 0;

        /** @brief 双目鱼眼标定程序
         *
        */
        virtual bool stereocalib_fisheye() = 0;
    };


}



#endif