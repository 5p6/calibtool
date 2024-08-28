import cv2
import open3d as o3d
import numpy as np
import argparse

def Monomain(args):
    # 获取旋转向量和平移向量
    RMatrixs = []
    tVectors = []
    colors_list = []  # 随机颜色
    with open(args.extrinsic_file_r,"r") as file:
        for line in file:
            dict_a = line.split(" ")
            mat = np.array([float(dict_a[0]),float(dict_a[1]),float(dict_a[2])],np.float32)
            # 旋转
            R,_= cv2.Rodrigues(mat)
            RMatrixs.append(R)
            # 颜色
            colors_list.append(np.random.rand(1, 3))
    with open(args.extrinsic_file_t,"r") as file:
        for line in file:
            dict_a = line.split(" ")
            mat = np.array([float(dict_a[0]),float(dict_a[1]),float(dict_a[2])],np.float32)
            # 平移
            tVectors.append(mat)
    
    # 世界点转换
    objp = np.zeros((1,args.board_size[0]*args.board_size[1],3),np.float32)
    objp[0,:,:2]  = np.mgrid[:args.board_size[0],:args.board_size[1]].T.reshape(-1,2)
    # 获取点云
    pcds = []
    points = []
    # 获取转换后的三维点
    for k in range(0,len(RMatrixs)):
        points = np.zeros((args.board_size[0]*args.board_size[1],3),np.float32)
        pcd = o3d.geometry.PointCloud()
        for i in range(0,objp.shape[1]):
            points[i,:] = np.dot(RMatrixs[k],objp[0,i,:]) + np.transpose(tVectors[k])
        pcd.points = o3d.utility.Vector3dVector(points)
        vertices  = []
        vertices.append(points[0,:])
        vertices.append(points[args.board_size[0] - 1,:])
        vertices.append(points[args.board_size[0] * args.board_size[1] - args.board_size[0] ,:])
        vertices.append(points[args.board_size[0] * args.board_size[1] - 1,:])
        vertices  = np.array(vertices)
        triangles = np.array([[2,1,0],[3,1,0],[1,2,3]],np.uint)
        # 创建TriangleMesh对象表示平面
        plane_mesh = o3d.geometry.TriangleMesh()
        plane_mesh.vertices = o3d.utility.Vector3dVector(vertices)
        plane_mesh.triangles = o3d.utility.Vector3iVector(triangles)
        # 颜色
        plane_mesh.paint_uniform_color([colors_list[k][0,0],colors_list[k][0,1],colors_list[k][0,2]])
        # 设置平面的颜色
        pcds.append(plane_mesh)
        pcds.append(pcd)
    Ordinate = o3d.geometry.TriangleMesh.create_coordinate_frame(size=5, origin=[0,0,0])
    pcds.append(Ordinate)
    o3d.visualization.draw_geometries(pcds)



if __name__=="__main__":
    parse = argparse.ArgumentParser()
    parse.add_argument("--extrinsic_file_r",default="./r.txt")
    parse.add_argument("--extrinsic_file_t",default="./t.txt")
    parse.add_argument("--board_size",default=(6,9),type=tuple)

    args = parse.parse_args()
    Monomain(args)
