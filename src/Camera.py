from .utiles import CalibrationConfig, \
                    parse_yaml, \
                    getCorner,\
                    getworldcornerpoints,\
                    visulizationCorner,\
                    load_world_points_from_csv,\
                    save_corners_points,\
                    save_world_points,\
                    load_pose_file,\
                    load_corner_from_csv,\
                    CalibrationConfig
import cv2
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import logging
from pathlib import Path
logging.basicConfig(
    level=logging.INFO,  # 可调为 DEBUG, WARNING, ERROR
    format="%(asctime)s [%(levelname)s] %(message)s",
    handlers=[
        logging.StreamHandler(),  # 输出到控制台
        logging.FileHandler("calibration.log", mode='w', encoding='utf-8')  # 输出到文件
    ]
)
logger = logging.getLogger(__name__)
class Camera(object):
    def __init__(self, config: CalibrationConfig) -> None:
        self.args = config
        logger.info("读取参数!")
        self.file = cv2.FileStorage(Path(config.output_dir) / "param.yaml",cv2.FILE_STORAGE_READ)
        # 相机型号
        self.Camera_SensorType = self.file.getNode("Camera_SensorType").string()
        self.Camera_NumType = self.file.getNode("Camera_NumType").string()
        self.height = int(self.file.getNode("height").real())
        self.width = int(self.file.getNode("width").real())
        
        # 左相机
        self.cam_matrix_left = self.file.getNode("K_l").mat()
        self.distortion_l = self.file.getNode("D_l").mat()

        if self.Camera_NumType == "Stereo":
            # 右相机
            self.cam_matrix_right = self.file.getNode("K_r").mat()
            self.distortion_r = self.file.getNode("D_r").mat()
            # 
            self.R = self.file.getNode("R").mat()
            self.t = self.file.getNode("t").mat()


        # 释放
        self.file.release()
        if self.Camera_NumType == "Monocular":
            if self.Camera_SensorType == "Fisheye" :
                self.map1x, self.map1y = cv2.fisheye.initUndistortRectifyMap(self.cam_matrix_left,self.distortion_l,None,self.cam_matrix_left,(self.width, self.height),cv2.CV_32FC1)
            elif self.Camera_SensorType == "Pinhole" :
                self.map1x, self.map1y = cv2.initUndistortRectifyMap(self.cam_matrix_left,self.distortion_l,None,self.cam_matrix_left,(self.width, self.height),cv2.CV_32FC1)
        elif self.Camera_NumType == "Stereo":
            # 畸变参数获取
            if self.Camera_SensorType == "Fisheye" :
                self.R1,self.R2,self.P1,self.P2,self.Q, validPixROI1, validPixROI2 = cv2.fisheye.stereoRectify(
                                                                                                                self.cam_matrix_left,
                                                                                                                self.distortion_l,
                                                                                                                self.cam_matrix_right,
                                                                                                                self.distortion_r,
                                                                                                                (self.width, self.height),
                                                                                                                self.R,
                                                                                                                self.t,
                                                                                                                flags= 0
                                                                                                                )
                self.map1x, self.map1y = cv2.fisheye.initUndistortRectifyMap(self.cam_matrix_left, self.distortion_l, self.R1, self.P1, (self.width, self.height), cv2.CV_32FC1)
                self.map2x, self.map2y = cv2.fisheye.initUndistortRectifyMap(self.cam_matrix_right, self.distortion_r, self.R2, self.P2, (self.width, self.height), cv2.CV_32FC1)
            elif self.Camera_SensorType == "Pinhole" :
                self.R1,self.R2,self.P1,self.P2,self.Q, validPixROI1, validPixROI2 = cv2.stereoRectify(
                                                                                                        self.cam_matrix_left,
                                                                                                        self.distortion_l,
                                                                                                        self.cam_matrix_right,
                                                                                                        self.distortion_r,
                                                                                                        (self.width, self.height),
                                                                                                        self.R,
                                                                                                        self.t,
                                                                                                        flags= 0,
                                                                                                        alpha= config.alpha
                                                                                                        )
                self.map1x, self.map1y = cv2.initUndistortRectifyMap(self.cam_matrix_left, self.distortion_l, self.R1, self.P1, (self.width, self.height), cv2.CV_32FC1)
                self.map2x, self.map2y = cv2.initUndistortRectifyMap(self.cam_matrix_right, self.distortion_r, self.R2, self.P2, (self.width, self.height), cv2.CV_32FC1)
        
    def rectify(self, left_img: np.ndarray, right_img: np.ndarray, line_interval: int = 40, colormap_name: str = "jet"):
        """
        畸变矫正+拼接+等距多彩横线可视化。

        Parameters:
            left_img (np.ndarray): 左图像
            right_img (np.ndarray): 右图像
            line_interval (int): 横线间隔
            colormap_name (str): 使用的 colormap 名称，默认 "jet"

        Returns:
            Tuple[np.ndarray, Optional[np.ndarray], Optional[np.ndarray]]
        """
        logger.info("畸变矫正!")
        concat_img = None

        if self.Camera_NumType == "Monocular":
            left_rectified = cv2.remap(left_img, self.map1x, self.map1y, interpolation=cv2.INTER_CUBIC, borderMode=cv2.BORDER_REPLICATE)
            right_rectified = None

        elif self.Camera_NumType == "Stereo":
            left_rectified = cv2.remap(left_img, self.map1x, self.map1y, interpolation=cv2.INTER_CUBIC, borderMode=cv2.BORDER_REPLICATE)
            right_rectified = cv2.remap(right_img, self.map2x, self.map2y, interpolation=cv2.INTER_CUBIC, borderMode=cv2.BORDER_REPLICATE)

            # 拼接
            concat_img = np.hstack((left_rectified, right_rectified))

            # 如果是灰度图像，先转为彩色
            if len(concat_img.shape) == 2 or concat_img.shape[2] == 1:
                concat_img = cv2.cvtColor(concat_img, cv2.COLOR_GRAY2BGR)

            # 获取 colormap 函数
            cmap = cm.get_cmap(colormap_name)

            # 添加彩色横线
            num_lines = concat_img.shape[0] // line_interval
            for i in range(num_lines):
                y = i * line_interval
                color = cmap(i / num_lines)[:3]  # RGBA -> RGB
                color_bgr = tuple(int(255 * c) for c in color[::-1])  # RGB to BGR for OpenCV
                cv2.line(concat_img, (0, y), (concat_img.shape[1], y), color_bgr, 1)

        return left_rectified, right_rectified, concat_img
    def compute_reprojection_errors(self):
        logger.info("输出重投影误差!")
        left_blank = np.ones((self.height,self.width,3),np.uint8)  * 255
        right_blank = np.ones((self.height,self.width,3),np.uint8)  * 255
        
        left_reproj_error = {}
        right_reproj_error = {}
        
        if self.Camera_NumType == "Monocular":
            # 加载图像角点
            left_corners_dcit= load_corner_from_csv(csv_dir=Path(self.args.image_points_dir) / "corners")
            left_pose_dcit = load_pose_file(Path(self.args.output_dir) / "pose.txt")
        elif self.Camera_NumType == "Stereo":
            # 加载图像角点
            left_corners_dcit = load_corner_from_csv(csv_dir=Path(self.args.image_points_dir) / "left_corners")
            left_pose_dcit = load_pose_file(Path(self.args.output_dir) / "left_pose.txt")
            right_corners_dcit = load_corner_from_csv(csv_dir=Path(self.args.image_points_dir) / "right_corners")
            right_pose_dcit = load_pose_file(Path(self.args.output_dir) / "right_pose.txt")
        
        # 加载世界点
        world_points = load_world_points_from_csv(self.args.world_points_file,len(left_corners_dcit))
        if self.Camera_NumType == "Monocular":
            # 加载图像角点
            for world_pts,(name, (rvec, tvec)) in zip(world_points, left_pose_dcit.items()):
                if self.Camera_SensorType == "Pinhole":
                    image_pts_reproj, _ = cv2.projectPoints(world_pts, rvec, tvec, self.cam_matrix_left, self.distortion_l) # 投影
                if self.Camera_SensorType == "Fisheye":
                    image_pts_reproj, _ = cv2.fisheye.projectPoints(world_pts, rvec, tvec, self.cam_matrix_left, self.distortion_l) # 投影
                image_pts_reproj = image_pts_reproj.reshape(-1,2)
                # 计算误差
                err = left_corners_dcit[name] - image_pts_reproj
                repro_err = np.sqrt(np.sum(err.flatten() ** 2) / len(err))  # 按行堆成1D向量
                left_reproj_error[name] = repro_err
                # 显示
                self.drawpoints(left_blank,image_pts_reproj,left_corners_dcit[name])
            logger.info("单目重投影误差直方图!")
            # 单目误差显示
            self.drawMonoErrorHistogram(left_reproj_error)
        elif self.Camera_NumType == "Stereo":
            # 加载图像角点
            for world_pts,(left_name, (left_rvec, left_tvec)),(right_name, (right_rvec, right_tvec)) in zip(world_points, left_pose_dcit.items(), right_pose_dcit.items()):
                if self.Camera_SensorType == "Pinhole":
                    left_pts_reproj, _ = cv2.projectPoints(world_pts, left_rvec, left_tvec, self.cam_matrix_left, self.distortion_l) # 投影
                    right_pts_reproj, _ = cv2.projectPoints(world_pts, right_rvec, right_tvec, self.cam_matrix_right, self.distortion_r) # 投影
                if self.Camera_SensorType == "Fisheye":
                    left_pts_reproj, _ = cv2.fisheye.projectPoints(world_pts, left_rvec, left_tvec, self.cam_matrix_left, self.distortion_l) # 投影
                    right_pts_reproj, _ = cv2.fisheye.projectPoints(world_pts, right_rvec, right_tvec, self.cam_matrix_right, self.distortion_r) # 投影
                left_pts_reproj = left_pts_reproj.reshape(-1,2)
                right_pts_reproj = right_pts_reproj.reshape(-1,2)
                
                # 计算误差
                left_err = left_corners_dcit[left_name] - left_pts_reproj
                right_err = right_corners_dcit[right_name] - right_pts_reproj
                left_repro_err = np.sqrt(np.sum(left_err.flatten() ** 2) / len(left_err))   # 按行堆成1D向量
                right_repro_err = np.sqrt(np.sum(right_err.flatten() ** 2) / len(right_err))  # 按行堆成1D向量
                # 每个误差推入
                left_reproj_error[left_name] = left_repro_err
                right_reproj_error[right_name] = right_repro_err
                # 显示
                self.drawpoints(left_blank,left_pts_reproj,left_corners_dcit[left_name])
                self.drawpoints(right_blank,right_pts_reproj,right_corners_dcit[right_name])
            logger.info("双目重投影误差直方图!")
            # 双目误差显示
            self.drawStereoErrorHistogram((left_reproj_error,right_reproj_error))
            cv2.imshow("right corners",right_blank)
        cv2.imshow("left corners",left_blank)
        logger.info("重投影点可视化,检测角点(绿圆),重投影角点(红叉)!")
        cv2.waitKey()

        return left_reproj_error,right_reproj_error
    
    def drawpoints(self,img,image_pts_reproj,left_corners):
        #  # 画图：红圈是重投影点，绿圈是真实角点
        for pt_proj, pt_corner in zip(image_pts_reproj, left_corners):
            # 红色叉：投影点
            pt_proj = np.int32(pt_proj)
            size = 5
            cv2.line(img, (pt_proj[0] - size, pt_proj[1] - size), (pt_proj[0] + size, pt_proj[1] + size), (0, 0, 255), thickness = 2)
            cv2.line(img, (pt_proj[0] - size, pt_proj[1] + size), (pt_proj[0] + size, pt_proj[1] - size), (0, 0, 255), thickness = 2)
                        
            # 绿色圆：角点
            cv2.circle(img, tuple(np.int32(pt_corner)), radius=11, color=(0, 255, 0), thickness= 2)
    
    def drawStereoErrorHistogram(self,data):
        # 统一图像名称顺序
        image_names = sorted(data[0].keys())  # 保证顺序一致
        values1 = [data[0][img] for img in image_names]
        values2 = [data[1][img] for img in image_names]

        x = np.arange(len(image_names))  # 横坐标索引
        width = 0.35  # 柱状图宽度

        # 开始画图
        fig, ax = plt.subplots(figsize=(10, 6))
        bar1 = ax.bar(x - width/2, values1, width, label='Left error', color='skyblue')
        bar2 = ax.bar(x + width/2, values2, width, label='Right error', color='lightcoral')

        # 加标签
        ax.set_xlabel('Image name')
        ax.set_ylabel('Reprojection error')
        ax.set_title('Reprojection error per image')
        ax.set_xticks(x)
        ax.set_xticklabels(image_names, rotation=45)
        ax.legend()
        ax.grid(True, linestyle='--', alpha=0.5)

        plt.tight_layout()
        plt.show()
    
    def drawMonoErrorHistogram(self, data):
        # 从字典中提取键和值
        image_files = list(data.keys())
        values = list(data.values())

        # 按照图像编号排序（从文件名中提取数字）
        sorted_indices = sorted(range(len(image_files)), key=lambda i: int(image_files[i].split('.')[0]))
        sorted_image_files = [image_files[i] for i in sorted_indices]
        sorted_values = [values[i] for i in sorted_indices]

        # 创建一个包含两个子图的图形
        fig, ax = plt.subplots(1, 1, figsize=(14, 6))

        # 条形图
        ax.bar(sorted_image_files, sorted_values, color='skyblue')
        ax.set_xlabel('Image name')
        ax.set_ylabel('Reprojection error')
        ax.set_title('Reprojection error per image')
        ax.grid(axis='y', linestyle='--', alpha=0.7)
        plt.tight_layout()
        plt.show()
    
    def transformTo3D(self, disp_img: np.ndarray) -> np.ndarray:
        return cv2.reprojectImageTo3D(disp_img, self.Q)
    