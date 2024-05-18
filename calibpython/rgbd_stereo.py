import cv2
import numpy as np
import argparse
import os
import numpy as np
from tqdm import tqdm


"用于显示校正后的双目图像是否对齐"
def cat(img1,img2):
    "左右视图都是一样大小"
    if img1.ndim==2:
        size=img1.shape
        img=np.zeros((size[0],size[1]*2))
        img[:,0:size[1]]=img1
        img[:,size[1]:2*size[1]]=img2
        for i in range(size[0]):
            if i%32==0:
                img[i,:]=0
    else:
        size=img1.shape
        img=np.zeros((size[0],size[1]*2,size[2]))
        img[:,0:size[1],:]=img1
        img[:,size[1]:2*size[1],:]=img2
        for i in range(size[0]):
            if i%32==0:
                img[i,:,:]=0
    return img.astype(np.uint8)

def main(args:argparse.Namespace):
    "得到左右图像的文件名"
    rgb_image_name = os.listdir(args.rgb_root_dir)
    infrared_image_name = os.listdir(args.infrared_root_dir)
    "图像的绝对路径"
    rgb_image_path = [os.path.join(args.rgb_root_dir,name) for name in  rgb_image_name]
    infrared_image_path = [os.path.join(args.infrared_root_dir,name) for name in  infrared_image_name]
    print("rgb相机图像数目 : ",len(rgb_image_path))
    print("红外相机图像数目 : ",len(infrared_image_path))
    # "终止条件"
    criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER,100,0)
    "棋盘格的尺寸大小"
    square = args.square_size
    "亚角点寻找半径"
    raduis = args.radius_size
    "棋盘格"
    chessboard = args.board_size
    chessboardsize = chessboard[0]*chessboard[1]
    "世界坐标点"
    objpoints=[]
    "图像坐标点"
    rgb_imgpoints=[]
    infrared_imgpoints=[]
    "世界点"
    objp=np.zeros((1,chessboardsize,3),np.float32)
    "创建棋盘格世界坐标"
    objp[0,:,:2]=np.mgrid[:chessboard[0],:chessboard[1]].T.reshape(-1,2)*square
    "棋盘格大小"

    index=0
    for i in tqdm(range(len(rgb_image_path))):
        "左右图像路径"
        rgb_path = rgb_image_path[i]
        infrared_path = infrared_image_path[i]
        "读取左右图像"
        rgb_image:np.ndarray = cv2.imread(rgb_path)
        infrared_image:np.ndarray = 255 -  cv2.imread(infrared_path)
        "灰度图"
        rgb_gray = cv2.cvtColor(rgb_image,cv2.COLOR_BGR2GRAY)
        infrared_gray = cv2.cvtColor(infrared_image, cv2.COLOR_BGR2GRAY)
        "ret是是否找到角点的标志,corners就是角点集合"
        if args.board_type=="Circle":
            rgb_ret, rgb_corners = cv2.findCirclesGrid(rgb_gray,chessboard)
            infrared_ret, infrared_corners = cv2.findCirclesGrid(infrared_gray, chessboard)
        elif args.board_type=="Corner":
            rgb_ret, rgb_corners = cv2.findChessboardCorners(rgb_gray,chessboard)
            infrared_ret, infrared_corners = cv2.findChessboardCorners(infrared_gray, chessboard)
        message = "未找到" if rgb_ret == False else "找到了"
        print("左图像路径 : ",rgb_path)
        print("右图像路径 : ",infrared_path)
        print("本次的角点 ",message)

        if ((infrared_ret and rgb_ret)==True):
            index += 1
            "亚像素角点做细化"
            if args.board_type=="Corner":
                rgb_corners  = cv2.cornerSubPix(rgb_gray,rgb_corners,raduis,(-1,-1),criteria)
                infrared_corners = cv2.cornerSubPix(infrared_gray, infrared_corners, raduis, (-1, -1), criteria)
            "显示"
            rgb_image = cv2.drawChessboardCorners(rgb_image, chessboard, rgb_corners, infrared_ret)
            infrared_image = cv2.drawChessboardCorners(infrared_image, chessboard, infrared_corners, infrared_ret)
            cv2.namedWindow("picure_rgb", cv2.WINDOW_NORMAL)
            cv2.namedWindow("picure_infrared", cv2.WINDOW_NORMAL)
            cv2.imshow("picure_rgb", rgb_image)
            cv2.imshow("picure_infrared", infrared_image)
            if  args.showdetail:
                print("just press the esc to next one")
                while(1):
                    if cv2.waitKey(1) == 27:
                        break
            else:
                cv2.waitKey(500)
            "添加世界点"
            objpoints.append(objp)
            "添加角点"
            rgb_imgpoints.append(rgb_corners)
            infrared_imgpoints.append(infrared_corners)
        else:
            print("已经移除")
    cv2.destroyAllWindows()
    "整理配对的角点"
    objpoints = np.array(objpoints).reshape(index,1,chessboardsize,3)
    rgb_imgpoints = np.array(rgb_imgpoints).reshape(index,1,chessboardsize,2)
    infrared_imgpoints = np.array(infrared_imgpoints).reshape(index,1,chessboardsize,2)
    


    h,w,z = rgb_image.shape
    "左相机标定"
    retval, cameraMatrix1, distCoeffs1, rvecs1, tvecs1 = cv2.calibrateCamera(objpoints,rgb_imgpoints,(w,h),cameraMatrix=None,distCoeffs=None,rvecs=None, tvecs=None,flags=0, criteria=criteria)
    print("RGB相机误差 : %f"%retval)
    "右相机标定"
    retval, cameraMatrix2, distCoeffs2, rvecs2, tvecs2 = cv2.calibrateCamera(objpoints,infrared_imgpoints,(w,h),cameraMatrix=None,distCoeffs=None,rvecs=None, tvecs=None, flags=0,criteria=criteria)
    print("配准相机误差：%f"%retval)
    print("RGB相机内参 : \n",cameraMatrix1)
    print("配准相机内参 : \n",cameraMatrix2)

    flag = 0
    # 用于固定内参
    flag += cv2.CALIB_FIX_INTRINSIC
    flag += cv2.CALIB_FIX_K3
    h,w,z = rgb_image.shape
    
    print("开始进行双目融合标定,计算双目的相关参数")
    "获取内参矩阵和畸变系数,R,T,E分别为旋转矩阵,平移矩阵,基本矩阵"
    retval, cameraMatrix1, distCoeffs1, cameraMatrix2, distCoeffs2, R, T, E, F = cv2.stereoCalibrate(
        objpoints,
        rgb_imgpoints,
        infrared_imgpoints,
        cameraMatrix1=cameraMatrix1,
        distCoeffs1=distCoeffs1,
        cameraMatrix2=cameraMatrix2,
        distCoeffs2=distCoeffs2,
        imageSize=(w,h),
        R=None,
        T=None,
        E=None,
        F=None,
        flags= flag,
        criteria=criteria
    )
    print("双相机误差：%f"%retval)
    print("K1 : \n",cameraMatrix1)
    print("D1 : \n",distCoeffs1)
    print("K2 : \n",cameraMatrix2)
    print("D2 : \n",distCoeffs2)
    print("the rotation matrix transform the rgb into infrared is:\n",R)
    print("the translation vector bewteen the O1 t0 O2:\n",T)

    "保存数据"
    "cameraMatrix1, distCoeffs1, cameraMatrix2, distCoeffs2"
    "R,T"
    if args.save_option:
        file = cv2.FileStorage(args.save_path, cv2.FILE_STORAGE_WRITE)
        print("本次将会把参数保存,并且保存到",args.save_path)
        "写入数据"
        file.write("Camera_SensorType","Pinhole")
        file.write("Camera_NumType","RGBD")
        file.write("K_l", cameraMatrix1)
        file.write("D_l", distCoeffs1)
        file.write("K_r", cameraMatrix2)
        file.write("D_r", distCoeffs2)
        file.write("R",R)
        file.write("T",T)
        file.write("height",h)
        file.write("width",w)
        file.release()
    else:
        print("本次将不会保存参数")
    




if __name__=="__main__":
    print("双目标定")
    parse = argparse.ArgumentParser()
    ## 图像根目录
    parse.add_argument("--rgb_root_dir",default="E:\\dataset\\calibrate\\rgb+infrared\\rgb+infrared\\rgb",help="rgb图像根目录")
    parse.add_argument("--infrared_root_dir",default="E:\\dataset\\calibrate\\rgb+infrared\\rgb+infrared\\infrared",help="红外图像(深度)根目录")
    ## 棋盘格查找和算法的一些选项
    parse.add_argument("-bt","--board_type",default="Corner",choices=["Corner","Circle"],help="棋盘格标定类型(角点、圆点)")
    parse.add_argument("-bs","--board_size",default= (7,6),type = int,nargs=2,help="标定板棋盘格规格")
    parse.add_argument("-s","--square_size",default= 25 ,type= float,help="棋盘格方格子尺寸")
    parse.add_argument("-rs","--radius_size",default=(7,7),type=int,nargs=2,help = "亚角点查找半径")
    # ## 效果显示的选项
    parse.add_argument("-sd","--showdetail",default=False,action="store_true",help="是否一直展示角点图像")
    ## 参数保存
    parse.add_argument("-so","--save_option",default=False,action="store_true",help= "是否保存参数")
    parse.add_argument("-sp","--save_path",default="./param/euroc.yaml",help="保存参数路径")

    args = parse.parse_args()
    main(args)