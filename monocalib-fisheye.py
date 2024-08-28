import os
import cv2
import numpy as np
import argparse

def main(args:argparse.Namespace):
    "图像路径文件夹"
    root_dir=args.root_dir
    image_name=os.listdir(root_dir)
    "图像的绝对路径"
    image_path=[os.path.join(root_dir,name) for name in  image_name]
    chessboard=args.board_size
    square = args.square_size
    chessboardsize=chessboard[0]*chessboard[1]
    "终止条件"
    criteria=(cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER,100,1e-6)
    "算法使用"
    flag=0
    # flag += cv2.fisheye.CALIB_RECOMPUTE_EXTRINSIC;
    # flag += cv2.fisheye.CALIB_FIX_SKEW
    # flag += cv2.fisheye.CALIB_FIX_INTRINSIC;
    # flag += cv2.fisheye.CALIB_FIX_PRINCIPAL_POINT;
    flag += cv2.fisheye.CALIB_RECOMPUTE_EXTRINSIC

    "亚角点寻找半径"
    raduis=args.radius_size
    "世界坐标点"
    objpoints=[]
    "图像坐标点"
    imgpoints=[]
    objp=np.zeros((1,chessboard[0]*chessboard[1],3),np.float32)
    "创建棋盘格世界坐标"
    objp[0,:,:2]=np.mgrid[:chessboard[0],:chessboard[1]].T.reshape(-1,2)*square


    index=0
    for path in image_path:
        image=cv2.imread(path)
        gray=cv2.cvtColor(image,cv2.COLOR_BGR2GRAY)
        if args.board_type=="Corner":
            "ret是是否找到角点的标志,corners就是角点集合"
            ret,corners=cv2.findChessboardCorners(gray,chessboard,cv2.CALIB_CB_ADAPTIVE_THRESH)
        elif args.board_type=="Circle":
            ret,corners=cv2.findCirclesGrid(gray,chessboard,cv2.CALIB_CB_ADAPTIVE_THRESH)
        if ret==True:
            print("角点查找成功,该图像的路径为 : ",path)
            index+=1
            "添加世界点"
            objpoints.append(objp)
            if args.board_type=="Corner":
                "亚像素角点做细化"
                corners = cv2.cornerSubPix(gray,corners,raduis,(-1,-1),criteria)
            new_image = cv2.drawChessboardCorners(image,chessboard,corners,ret)
            cv2.imshow("picure",new_image)
            if args.showdetail==0:
                cv2.waitKey(500)
            else:
                while 1:
                    if(cv2.waitKey(1)==27):
                        break
            "添加角点"
            imgpoints.append(corners)
    objpoints=np.array(objpoints).reshape(index,1,chessboardsize,3)
    imgpoints=np.array(imgpoints).reshape(index,1,chessboardsize,2)

    "图像大小"
    h,w=gray.shape[:2]
    "得到内参矩阵K和畸变系数D"
    retval,K,D,rvecs,tvecs=cv2.fisheye.calibrate(
        objpoints,
        imgpoints,
        (w,h),
        K=None,
        D=None,
        rvecs=None, 
        tvecs=None, 
        flags=flag,
        criteria=criteria
        )
    print("相机标定误差为 : ",retval)
    print("内参矩阵为 : \n",K)
    print("畸变参数矩阵为 : \n",D)
    if args.saveoption:
        print("参数会保存到文件 : ",args.save_path)
        file:cv2.FileStorage = cv2.FileStorage(args.save_path,cv2.FILE_STORAGE_WRITE)
        file.write("Camera_SensorType","Fisheye")
        file.write("Camera_NumType","Monocular")
        file.write("K",K)
        file.write("D",D)
        file.write("height",h)
        file.write("width",w)
        file.release()
    else:
        print("本次参数不会保存")

    "映射表"
    print("计算映射表！")
    mapx,mapy=cv2.fisheye.initUndistortRectifyMap(K,D,np.eye(3),K,(w,h),cv2.CV_32FC1)
    
    "开始矫正"
    print("开始矫正图像")
    image=cv2.imread(image_path[0])
    "矫正"
    image_new=cv2.remap(image,mapx,mapy,interpolation=cv2.INTER_CUBIC,borderMode=cv2.BORDER_REPLICATE)

    # image=cv2.resize(image,None,fx=0.5,fy=0.5,interpolation=cv2.INTER_LINEAR)
    # image_new=cv2.resize(image_new,None,fx=0.5,fy=0.5,interpolation=cv2.INTER_LINEAR)
    cv2.namedWindow("src",cv2.WINDOW_NORMAL)
    cv2.namedWindow("dst",cv2.WINDOW_NORMAL)
    cv2.imshow("src",image)
    cv2.imshow("dst",image_new)
    print("just press esc to retire !")
    cv2.waitKey(0)


if __name__=="__main__":
    parse = argparse.ArgumentParser()
    parse.add_argument("-so","--saveoption",default= False,action="store_true",help = "是否保存为xml文件")
    parse.add_argument("-sp","--save_path",default= "E:\\dataset\\calibrate\\aruco_monocular\\family\\xml\\sixth.xml",help="内外参保存文件名")
    parse.add_argument("-r","--root_dir",default= "E:\\dataset\\calibrate\\fisheye\\chessboard",help="标定图像的根目录")
    
    parse.add_argument("-bt","--board_type",default="Corner",choices=["Corner","Circle"],help="棋盘格标定类型(角点、圆点)")
    parse.add_argument("-bs","--board_size",default= (7,6),type = int,nargs=2,help="棋盘格规格(m n)")
    parse.add_argument("-s","--square_size",default= 20 ,type= float,help="棋盘格尺寸(m)")
    parse.add_argument("-rs","--radius_size",default=(1,1),type = int,nargs = 2,help = "亚像素角点查找半径(m n)")
    parse.add_argument("-sd","--showdetail",default=False,action="store_true",help="是否显示具体")
    args=parse.parse_args()
    main(args)


