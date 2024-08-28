import os
import cv2
import numpy as np
import argparse

def main(args:argparse.Namespace):
    stereo = cv2.FileStorage(args.param_path,cv2.FileStorage_WRITE)
    left_file = cv2.FileStorage(args.Left_Intrinsic,cv2.FileStorage_READ)
    right_file = cv2.FileStorage(args.Right_Intrinsic,cv2.FileStorage_READ)
    extrinsic_file = cv2.FileStorage(args.Extrinsic_path,cv2.FileStorage_READ)
    # if left_file.getNode("Camera_SensorType") != "Pinhole":
    #     return False
    print(left_file.getNode("width").real())
    K1 = np.array([
        [left_file.getNode("projection_parameters.fx").real(), 0., left_file.getNode("projection_parameters.cx").real()],
        [0., left_file.getNode("projection_parameters.fy").real(), left_file.getNode("projection_parameters.cy").real()],
        [0., 0., 1.]
    ],np.float32)
    print(K1)
    K2 = np.array([
        [right_file.getNode("projection_parameters.fx").real(), 0., right_file.getNode("projection_parameters.cx").real()],
        [0., right_file.getNode("projection_parameters.fy").real(), right_file.getNode("projection_parameters.cy").real()],
        [0., 0., 1.]
    ],np.float32)
    print(K2)
    
    stereo.write("K1",K1)
    stereo.write("K2",K2)
    stereo.release()
    return True
    
    

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--NumType",default="Monocular",help="")
    parser.add_argument("--Left_Intrinsic",default="param/camera_left.yaml",help="")
    parser.add_argument("--Right_Intrinsic",default="param/camera_right.yaml",help="")
    parser.add_argument("--Extrinsic_path",default="param/camera.yaml")
    parser.add_argument("--param_path",default="param/stereo.yaml")
    args = parser.parse_args()
    main(args)