import cv2
import numpy as np
from typing import Tuple
from matplotlib import pyplot as plt

# 双目相机参数
class stereoCamera(object):
    def __init__(self,param_path) -> None:
        self.file = cv2.FileStorage(param_path,cv2.FILE_STORAGE_READ)
        self.Camera_type = self.file.getNode("Camera_SensorType").string()
        # 左相机内参
        self.cam_matrix_left = self.file.getNode("K_l").mat()
        # 右相机内参
        self.cam_matrix_right = self.file.getNode("K_r").mat()

        # 左右相机畸变系数:[k1, k2, p1, p2, k3]
        self.distortion_l = self.file.getNode("D_l").mat()
        self.distortion_r = self.file.getNode("D_r").mat()

        # 旋转矩阵
        self.R1 = self.file.getNode("R_l").mat()
        self.R2 = self.file.getNode("R_r").mat()
        # 平移矩阵
        self.P1 = self.file.getNode("P_l").mat()
        self.P2 = self.file.getNode("P_r").mat()        
        # 主点列坐标的差
        # self.doffs = 26.23
        # 重投影矩阵
        self.Q = self.file.getNode("Q").mat()
        self.height = int(self.file.getNode("height").real())
        self.width = int(self.file.getNode("width").real())
        # 释放
        self.file.release()
        # 畸变参数获取
        if self.Camera_type == "Fisheye" :
            self.map1x, self.map1y = cv2.fisheye.initUndistortRectifyMap(self.cam_matrix_left, self.distortion_l, self.R1, self.P1, (self.width, self.height), cv2.CV_32FC1)
            self.map2x, self.map2y = cv2.fisheye.initUndistortRectifyMap(self.cam_matrix_right, self.distortion_r, self.R2, self.P2, (self.width, self.height), cv2.CV_32FC1)
        elif self.Camera_type == "Pinhole" :
            self.map1x, self.map1y = cv2.initUndistortRectifyMap(self.cam_matrix_left, self.distortion_l, self.R1, self.P1, (self.width, self.height), cv2.CV_32FC1)
            self.map2x, self.map2y = cv2.initUndistortRectifyMap(self.cam_matrix_right, self.distortion_r, self.R2, self.P2, (self.width, self.height), cv2.CV_32FC1)
        

    # 矫正
    def rectify(self,left_img :np.ndarray ,right_img: np.ndarray) -> Tuple[np.ndarray , np.ndarray]:
        print("distortion rectify !")
        # 矫正
        left_rectified = cv2.remap(left_img, self.map1x, self.map1y, interpolation=cv2.INTER_CUBIC,borderMode=cv2.BORDER_REPLICATE)
        right_rectified = cv2.remap(right_img, self.map2x, self.map2y, interpolation=cv2.INTER_CUBIC,borderMode=cv2.BORDER_REPLICATE)
        return left_rectified,right_rectified
    
    # 转换为点云图
    def transformTo3D(self,disp_img :np.ndarray ,Q:np.ndarray) -> np.ndarray :
        return cv2.reprojectImageTo3D(disp_img,Q)
