import cv2
import numpy as np


# 单目
class MonoCamera:
    def __init__(self,param_file) -> None:
        self.file:cv2.FileStorage = cv2.FileStorage(param_file,cv2.FileStorage_READ)
        # 参数
        self.Camera_type = self.file.getNode("Camera_SensorType").string()
        self.K = self.file.getNode("K_l").mat()
        self.D = self.file.getNode("D_l").mat()
        self.height = int(self.file.getNode("height").real())
        self.width = int(self.file.getNode("width").real())
        if self.Camera_type == "Pinhole":
            self.mapx,self.mapy = cv2.initUndistortRectifyMap(self.K,self.D,None,self.K,(self.width,self.height),cv2.CV_32FC1)
        elif self.Camera_type=="Fisheye":
            self.mapx,self.mapy = cv2.fisheye.initUndistortRectifyMap(self.K,self.D,None,self.K,(self.width,self.height),cv2.CV_32FC1)

    # 矫正
    def rectify(self,img):
        rectify_img = cv2.remap(img,self.mapx,self.mapy,cv2.INTER_CUBIC)
        return rectify_img
    
    # 深度图转换
    def convert_from_uvd(self, u, v, d):
        # d *= self.pxToMetre
        x_over_z = (self.K[0][2] - u) / self.K[0][0]
        y_over_z = (self.K[1][2] - v) / self.K[1][1]
        z = d 
        x = x_over_z * z
        y = y_over_z * z
        return x, y, z
    

    # 深度映射到图像
    def depthTO3D(self,depth:np.ndarray):
        """
        图像的坐标轴:
        ---->x
        |
        |
        y
        depth : 图像的深度图 ,Z
        K : 内参矩阵
        K =  f_x  0  c_x 
            0   f_y c_y
            0    0   1
        X = Z / f * (x - c_x) 
        Y = Z / f * (y - c_y)
        """
        # 利用内参重投影
        points:np.ndarray = np.zeros((depth.shape[0],depth.shape[1],3),np.float64)
        
        for i in range(0,depth.shape[0]):
            for j in range(0,depth.shape[1]):
                # # x
                points[i][j][0] = depth[i][j] * (i - self.K[0][2]) / self.K[0][0]
                # y
                points[i][j][1] = depth[i][j] * (j - self.K[1][2]) / self.K[1][1]
                # z
                points[i][j][2] = depth[i][j]
        return points


