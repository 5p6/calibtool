import cv2
import numpy as np
import argparse
import os


def main(args:argparse.Namespace):
    root_dir=args.root_dir
    image_name=os.listdir(root_dir)
    "图像的绝对路径"
    image_path=[os.path.join(root_dir,name) for name in  image_name]
    square = args.square_size
    chessboard=args.board_size
    chessboardsize=chessboard[0]*chessboard[1]
    "终止条件"
    criteria=(cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER,100,1e-7)
    "算法使用"
    flag=0
    # flag  |= cv2.omnidir.CALIB_FIX_SKEW
    "亚角点寻找半径"
    raduis=args.radius_size
    "世界坐标点"
    objpoints=[]
    "图像坐标点"
    imgpoints=[]
    objp=np.zeros((1,chessboard[0]*chessboard[1],3),np.float32)
    "创建棋盘格世界坐标"
    objp[0,:,:2]  = np.mgrid[:chessboard[0],:chessboard[1]].T.reshape(-1,2)*square

    index=0
    for path in image_path:
        image=cv2.imread(path)
        gray = cv2.cvtColor(image,cv2.COLOR_BGR2GRAY)
        "缩小尺寸"
        # gray = cv2.resize(gray,None,fx=0.25,fy=0.25)
        if args.board_type=="Corner":
            "ret是是否找到角点的标志,corners就是角点集合"
            ret,corners=cv2.findChessboardCorners(gray,chessboard,cv2.CALIB_CB_ADAPTIVE_THRESH)
        elif args.board_type=="Circle":
            ret,corners=cv2.findCirclesGrid(gray,chessboard,cv2.CALIB_CB_ADAPTIVE_THRESH)
        if ret==True:
            print("图像 :",path," 已经找到角点,现在做亚角点处理")
            index+=1
            "添加世界点"
            objpoints.append(objp)
            if args.board_type=="Corner":
                "亚像素角点做细化"
                corners = cv2.cornerSubPix(gray,corners,raduis,(-1,-1),criteria)
            "添加角点"
            imgpoints.append(corners)
            newimage = cv2.drawChessboardCorners(gray,chessboard,corners,ret)
            if args.showdetail==1:
                print("just prees the est to show next one")
                cv2.namedWindow("picure",cv2.WINDOW_NORMAL)
                cv2.imshow("picure",newimage)
                while True:
                    if(cv2.waitKey(1)==27):
                        break
            else:
                cv2.namedWindow("picure",cv2.WINDOW_NORMAL)
                cv2.imshow("picure",newimage)
                cv2.waitKey(500)
    cv2.destroyAllWindows()
    "调整世界点和角点的大小"
    objpoints=np.array(objpoints).reshape(index,1,chessboardsize,3)
    imgpoints=np.array(imgpoints).reshape(index,1,chessboardsize,2)

    "图像大小"
    h,w=gray.shape[:2]
    "得到内参矩阵K和畸变系数D"
    retval, K, xi , D , rvecs, tvecs , idx = cv2.omnidir.calibrate(
        objpoints,
        imgpoints,
        (w,h),
        K=None,
        xi=None,
        D=None,
        flags=flag,
        criteria=criteria,
        rvecs=None, 
        tvecs=None,
        idx=None
    )
    print("误差 : ",retval)
    print("内参矩阵 K : \n",K)
    print("xi : ",xi)
    print("畸变参数 : \n",D)

    if args.saveoption == 1:
        print("参数会保存到文件 : ",args.save_path)
        file:cv2.FileStorage = cv2.FileStorage(args.save_path,cv2.FILE_STORAGE_WRITE)
        file.write("Camera_SensorType","Omnidir")
        file.write("Camera_NumType","Monocular")
        file.write("K",K)
        file.write("D",D)
        file.write("xi",xi)
        file.write("height",h)
        file.write("width",w)
        file.release()
    else:
        print("本次参数不会保存")


    "矫正示例"
    image: np.ndarray =cv2.imread(image_path[0])
    
    "映射表"
    mapx,mapy = cv2.omnidir.initUndistortRectifyMap(K,D,xi,np.eye(3,3),K,(w,h),cv2.CV_32FC1,cv2.omnidir.RECTIFY_PERSPECTIVE)
    
    "矫正"
    image_new = cv2.remap(image,mapx,mapy,interpolation=cv2.INTER_LINEAR,borderMode=cv2.BORDER_CONSTANT)

    cv2.namedWindow("src", cv2.WINDOW_NORMAL)
    cv2.namedWindow("dst", cv2.WINDOW_NORMAL)
    cv2.imshow("src",image)
    cv2.imshow("dst",image_new)
    cv2.waitKey(0)




if __name__ == "__main__":
    parse = argparse.ArgumentParser()
    parse.add_argument("-so","--saveoption",default= False,action="store_true",help = "是否保存为xml文件")
    parse.add_argument("-sp","--save_path",default= "E:\\dataset\\calibrate\\aruco_monocular\\family\\xml\\sixth.xml",help="内外参保存文件名")
    parse.add_argument("-r","--root_dir",default= "E:\\dataset\\calibrate\\fisheye\\chessboard",help="标定图像根目录")


    parse.add_argument("-bt","--board_type",default="Corner",choices=["Corner","Circle"],help="棋盘格标定类型(角点、圆点)")
    parse.add_argument("-bs","--board_size",default= (7,6),type = int,nargs=2,help="棋盘格规格")
    parse.add_argument("-s","--square_size",default= 20 ,type= float,help="棋盘格尺寸")
    parse.add_argument("-rs","--radius_size",default=(11,11),type = int,nargs=2,help = "亚角点查找半径")
    parse.add_argument("-sd","--showdetail",default=False,action="store_true",help="是否一直显示角点图像")
    args=parse.parse_args()
    
    main(args)






