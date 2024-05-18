import cv2
import numpy as np
import argparse
import os
import numpy as np
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
    "左右图像路径文件夹路径,文件夹路径下都是你的图片"
    left_root_dir = args.left_root_dir
    right_root_dir = args.right_root_dir
    "得到左右图像的文件名"
    left_image_name = os.listdir(left_root_dir)
    right_image_name = os.listdir(right_root_dir)
    "图像的绝对路径"
    left_image_path = [os.path.join(left_root_dir,name) for name in  left_image_name]
    right_image_path = [os.path.join(right_root_dir,name) for name in  right_image_name]
    "终止条件"
    criteria=(cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER,100,1e-7)
    "算法使用"
    flag = 0
    # flag += cv2.fisheye.CALIB_RECOMPUTE_EXTRINSIC
    flag += cv2.fisheye.CALIB_FIX_SKEW
    # flag += cv2.fisheye.CALIB_FIX_INTRINSIC
    # flag += cv2.fisheye.CALIB_FIX_PRINCIPAL_POINT
    flag += cv2.fisheye.CALIB_RECOMPUTE_EXTRINSIC

    "小棋盘格子的尺寸"
    square = args.square_size
    "亚角点寻找半径"
    raduis = args.radius_size
    "棋盘格"
    chessboard = args.board_size
    chessboardsize = chessboard[0]*chessboard[1]
    "世界坐标点"
    objpoints = []
    "图像坐标点"
    left_imgpoints = []
    right_imgpoints = []
    "世界点"
    objp = np.zeros((1,chessboardsize,3),np.float32)
    "创建棋盘格世界坐标"
    objp[0,:,:2] = np.mgrid[:chessboard[0],:chessboard[1]].T.reshape(-1,2)*square
    "棋盘格大小"
    index = 0
    for i in range(len(left_image_path)):
        "左右图像路径"
        left_path = left_image_path[i]
        right_path = right_image_path[i]
        "读取左右图像"
        left_image = cv2.imread(left_path)
        right_image = cv2.imread(right_path)
        if args.img_resizeoption:
            left_image = cv2.resize(left_image,None,fx=args.resize_times,fy=args.resize_times)
            right_image = cv2.resize(right_image,None,fx=args.resize_times,fy=args.resize_times)
        "灰度图"
        left_gray = cv2.cvtColor(left_image,cv2.COLOR_BGR2GRAY)
        right_gray = cv2.cvtColor(right_image, cv2.COLOR_BGR2GRAY)
        if args.board_type=="Corner":
            "ret是是否找到角点的标志,corners就是角点集合"
            left_ret, left_corners = cv2.findChessboardCorners(left_gray,chessboard,None , cv2.CALIB_CB_ADAPTIVE_THRESH | cv2.CALIB_CB_FILTER_QUADS)
            right_ret, right_corners = cv2.findChessboardCorners(right_gray, chessboard, None ,cv2.CALIB_CB_ADAPTIVE_THRESH | cv2.CALIB_CB_FILTER_QUADS)
        elif args.board_type=="Circle":
            left_ret, left_corners = cv2.findCirclesGrid(left_gray,chessboard)
            right_ret, right_corners = cv2.findCirclesGrid(right_gray, chessboard)
        if (right_ret==left_ret==True):
            print("角点找到了")
            index+=1
            "添加世界点"
            if args.board_type=="Corner":
                "亚像素角点做细化"
                left_corners  = cv2.cornerSubPix(left_gray,left_corners,raduis,(-1,-1),criteria)
                right_corners = cv2.cornerSubPix(right_gray, right_corners, raduis, (-1, -1), criteria)
            # "显示"
            left_image=cv2.drawChessboardCorners(left_image,chessboard,left_corners,left_ret)
            right_image=cv2.drawChessboardCorners(right_image, chessboard, right_corners,right_ret)
            print("左图像路径 : ",left_path)
            print("右图像路径 : ",right_path)
            cv2.imshow("picure1", left_image)
            cv2.imshow("picure2", right_image)
            if args.showdetail:
                while True:
                    "延迟半秒"
                    if(cv2.waitKey(1) ==27):
                        break
            else:
                cv2.waitKey(500)
            "添加角点"
            objpoints.append(objp)
            left_imgpoints.append(left_corners)
            right_imgpoints.append(right_corners)
    cv2.destroyAllWindows()
    "将角点和世界点的格式重新调整为相同格式"
    objpoints = np.array(objpoints).reshape(index,1,chessboardsize,3)
    left_imgpoints = np.array(left_imgpoints).reshape(index,1,chessboardsize,2)
    right_imgpoints = np.array(right_imgpoints).reshape(index,1,chessboardsize,2)
    "需要矫正的图片读取"
    h,w,z=left_image.shape

    "左相机标定"
    left_ret_l,K1,D1,rvecs1,tvecs1=cv2.fisheye.calibrate(objpoints,left_imgpoints,(w,h),K=None,D=None,rvecs=None, tvecs=None, flags=flag,criteria=criteria)
    print("左相机误差 : %f"%left_ret_l)
    "右相机标定"
    right_ret_r,K2,D2,rvecs2,tvecs2=cv2.fisheye.calibrate(objpoints,right_imgpoints,(w,h),K=None,D=None,rvecs=None, tvecs=None, flags=flag,criteria=criteria)
    print("右相机误差 : %f"%right_ret_r)
    "获取内参矩阵K和畸变系数D,R,T分别为旋转矩阵,平移向量"
    print("右相机内参 : \n",K1,"\n左相机内参 : \n",K2)
    print("右相机畸变 : \n",D1,"\n左相机畸变 : \n",D2)
    flag = 0
    # flag  |= cv2.fisheye.CALIB_RECOMPUTE_EXTRINSIC
    # flag  |= cv2.fisheye.CALIB_CHECK_COND
    # flag  |= cv2.fisheye.CALIB_FIX_SKEW
    # try:
    "鱼眼双目标定"
    retval, K1, D1, K2, D2, R, T,rvecs,tvecs= cv2.fisheye.stereoCalibrate(
            objpoints,
            left_imgpoints,
            right_imgpoints,
            K1=K1,
            D1=D1,
            K2=K2,
            D2=D2,
            imageSize=(w,h),
            R =None,
            T= None,
            rvecs = None,
            tvecs = None,
            flags=flag,
            criteria=(3,100,0)
            )
    # except Exception as e:
    #     print(f"Failed with message: {e}")
    
    print("双相机误差 : %f"%retval)
    print("右相机内参 : \n",K1,"\n左相机内参 : \n",K2)
    print("右相机畸变 : \n",D1,"\n左相机畸变 : \n",D2)
    print("双目相机的旋转矩阵 : \n",R)
    print("双目相机的平移向量 : \n",T)

    "找对应的旋转矩阵和新的相机内参矩阵"
    "计算矫正需要的参数"
    R1, R2, P1, P2, Q=cv2.fisheye.stereoRectify(
        K1, 
        D1, 
        K2, 
        D2, 
        (w,h), 
        R, 
        T,
        cv2.fisheye.CALIB_ZERO_DISPARITY
        )
    # R1, R2, P1, P2, Q=cv2.fisheye.stereoRectify(K1, D1, K2, D2, (w,h), R, T,0)
    "参数保存"
    if args.save_option:
        file = cv2.FileStorage(args.save_path, cv2.FileStorage_WRITE)
        file.write("Camera_SensorType","Fisheye")
        file.write("Camera_NumType","Stereo")
        file.write("K_l", K1)
        file.write("D_l", D1)
        file.write("K_r", K2)
        file.write("D_r", D2)
        file.write("R",R)
        file.write("T",T)
        # file.write("R_l", R1)
        # file.write("R_r", R2)
        # file.write("P_l", P1)
        # file.write("P_r", P2)
        # file.write("Q", Q)
        file.write("height",h)
        file.write("width",w)
        file.release()
    else:
        print("本次不会保存数据")


    ## 畸变矫正测试
    left_image = cv2.imread(left_image_path[args.test_index])
    right_image = cv2.imread(right_image_path[args.test_index])
    if args.img_resizeoption:
        left_image = cv2.resize(left_image,None,fx=args.resize_times,fy=args.resize_times)
        right_image = cv2.resize(right_image,None,fx=args.resize_times,fy=args.resize_times)
    "开始矫正"
    "左右映射图"
    left_map1,left_map2=cv2.fisheye.initUndistortRectifyMap(K1, D1, R1, P1,(w,h),cv2.CV_32FC1)
    right_map1,right_map2=cv2.fisheye.initUndistortRectifyMap(K2, D2, R2, P2,(w,h),cv2.CV_32FC1)
    "矫正图"
    left = cv2.remap(left_image,left_map1,left_map2,interpolation=cv2.INTER_CUBIC,borderMode=cv2.BORDER_REPLICATE)
    right = cv2.remap(right_image,right_map1,right_map2,interpolation=cv2.INTER_CUBIC,borderMode=cv2.BORDER_REPLICATE)
    picure=cat(left,right)
    ## 保存畸变图像
    if args.save_img:
        cv2.imwrite("./rect/left_fish.jpg",left)
        cv2.imwrite("./rect/right_fish.jpg",right)
    ## 显示
    cv2.namedWindow("left",cv2.WINDOW_NORMAL)
    cv2.namedWindow("right",cv2.WINDOW_NORMAL)
    cv2.namedWindow("concation",cv2.WINDOW_NORMAL)
    cv2.imshow("left",left)
    cv2.imshow("right",right)
    cv2.imshow("concation",picure)
    cv2.waitKey(0)


if __name__=="__main__":
    parse = argparse.ArgumentParser()
    ## 图像根目录
    parse.add_argument("-l","--left_root_dir",default="E:\\dataset\\calibrate\\fishpicure\\calibrate\\left",help="左图像根目录")
    parse.add_argument("-r","--right_root_dir",default="E:\\dataset\\calibrate\\fishpicure\\calibrate\\right",help="右图像根目录")
    ## 棋盘格查找和算法的一些选项
    parse.add_argument("-bt","--board_type",default="Corner",choices=["Corner","Circle"],help="棋盘格标定类型(角点、圆点)")
    parse.add_argument("-bs","--board_size",default= (7,6),type = int,nargs=2,help="棋盘格规格")
    parse.add_argument("-s","--square_size",default= 20 ,type= float,help="棋盘格方格子尺寸")
    parse.add_argument("-rs","--radius_size",default=(5,5),type=int,nargs=2,help = "亚像素角点查找半径")
    ## 效果显示的选项
    parse.add_argument("-sd","--showdetail",default=False,action="store_true",help="角点图像是否一直显示")
    ## 参数保存
    parse.add_argument("-so","--save_option",default=False,action="store_true",help= "是否保存标定结果参数")
    parse.add_argument("-sp","--save_path",default="./param/fisheye.yaml",help="参数保存路径")
    ## 去畸变示例和保存畸变矫正图像
    parse.add_argument("-ind","--test_index",default=2,type=int,help="展示畸变图像的索引")
    parse.add_argument("-sim","--save_img",default=0,type=int,help="是否保存校正图像")
    ## 图像尺寸重定义,为了适配立体匹配算法
    parse.add_argument("--img_resizeoption",default=False,action="store_true",help="是否重整图像尺度")
    parse.add_argument("--resize_times",default=0.5,type=float,help= "重整图像尺度倍数")
    args = parse.parse_args()
    main(args)
    