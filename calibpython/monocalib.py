import cv2
import numpy as np
import os
import argparse


def main(args:argparse.Namespace):
    root_dir = args.root_dir
    image_name = os.listdir(root_dir)
    "图像的绝对路径"
    image_path = [os.path.join(root_dir,name) for name in  image_name]
    square = args.square_size
    chessboard = args.board_size
    chessboardsize = chessboard[0]*chessboard[1]
    "终止条件"
    criteria=(cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER,100,1e-7)
    "算法使用"
    flag = 0
    # flag |=cv2.CALIB_FIX_PRINCIPAL_POINT
    # flag |=cv2.CALIB_FIX_ASPECT_RATIO
    "亚角点寻找半径"
    raduis=args.radius_size
    "世界坐标点"
    objpoints=[]
    "图像坐标点"
    imgpoints=[]
    objp = np.zeros((1,chessboard[0]*chessboard[1],3),np.float32)
    "创建棋盘格世界坐标"
    objp[0,:,:2]  = np.mgrid[:chessboard[0],:chessboard[1]].T.reshape(-1,2)*square

    index=0
    for path in image_path:
        image= cv2.imread(path)
        # cv2.imwrite(os.path.join("E:\\dataset\\calibrate\\rgb+infrared\\rgbdslam\\infrarednew",image_name[index]),image)
        gray = cv2.cvtColor(image,cv2.COLOR_BGR2GRAY)
        # gray = cv2.resize(gray,(640,480))
        # 尺寸缩小的问题
        # if args.resize_option==1:
            # gray = cv2.resize(gray,None,fx=args.resize_times,fy=args.resize_times)
        if args.board_type=="Corner":
            "ret是是否找到角点的标志,corners就是角点集合"
            ret,corners=cv2.findChessboardCorners(gray,chessboard,cv2.CALIB_CB_ADAPTIVE_THRESH)
        elif args.board_type=="Circle":
            ret,corners=cv2.findCirclesGrid(gray,chessboard,cv2.CALIB_CB_ADAPTIVE_THRESH)
        message = "找到了" if ret==True else "没找到"
        print("图像 :",path,message,"角点")
        if ret==True:
            index+=1
            "添加世界点"
            objpoints.append(objp)
            if args.board_type=="Corner":
                "亚像素角点做细化"
                corners = cv2.cornerSubPix(gray,corners,raduis,(-1,-1),criteria)
            "添加角点"
            imgpoints.append(corners)
            newimage = cv2.drawChessboardCorners(gray,chessboard,corners,ret)
            if args.showdetail:
                print("just prees the est to show next one")
                # cv2.namedWindow("picure",cv2.WINDOW_NORMAL)
                cv2.imshow("picure",newimage)
                while True:
                    if(cv2.waitKey(1)==27):
                        break
            else:
                # cv2.namedWindow("picure",cv2.WINDOW_NORMAL)
                cv2.imshow("picure",newimage)
                cv2.waitKey(500)
    cv2.destroyAllWindows()
    "调整世界点和角点的大小"
    objpoints=np.array(objpoints).reshape(index,1,chessboardsize,3)
    imgpoints=np.array(imgpoints).reshape(index,1,chessboardsize,2)

    "图像大小"
    h,w=gray.shape[:2]
    print("尺寸 : ",h,w)
    "得到内参矩阵K和畸变系数D"
    retval, cameraMatrix, distCoeffs, rvecs, tvecs = cv2.calibrateCamera(
        objpoints,
        imgpoints,
        (w,h),
        cameraMatrix=None,
        distCoeffs=None,
        rvecs=None, 
        tvecs=None, 
        flags = flag,
        criteria = criteria
    )
    "如果缩小了尺寸,就不要注释下面代码"
    # cameraMatrix  = 4 * cameraMatrix
    # distCoeffs = np.zeros((1,5))
    print("误差 : ",retval)
    print("内参矩阵 K : \n",cameraMatrix)
    print("畸变参数",distCoeffs)

    if args.saveoption == 1:
        print("参数会保存到文件 : ",args.save_path)
        file:cv2.FileStorage = cv2.FileStorage(args.save_path,cv2.FILE_STORAGE_WRITE)
        file.write("Camera_SensorType","Pinhole")
        file.write("Camera_NumType","Monocular")
        file.write("K",cameraMatrix)
        file.write("D",distCoeffs)
        file.write("height",h)
        file.write("width",w)
        file.release()
        # 外参
        # 旋转
        with open(args.extrinsic_file_r,"w") as file:
            for r in rvecs:
                file.write("{} {} {} \n".format(r[0].item(),r[1].item(),r[2].item()))
        # 平移
        with open(args.extrinsic_file_t,"w") as file:
            for t in tvecs:
                file.write("{} {} {} \n".format(t[0].item(),t[1].item(),t[2].item()))
    else:
        print("本次参数不会保存")
        # 旋转

    "矫正示例"
    image: np.ndarray =cv2.imread(image_path[0])
    if args.resize_option:
            image = cv2.resize(image,None,fx=args.resize_times,fy=args.resize_times)
    "映射表"
    mapx,mapy = cv2.initUndistortRectifyMap(cameraMatrix,distCoeffs,None,None,(w,h),cv2.CV_32FC1)
    "矫正"
    image_new = cv2.remap(image,mapx,mapy,interpolation=cv2.INTER_CUBIC)

    # cv2.namedWindow("src", cv2.WINDOW_NORMAL)
    # cv2.namedWindow("dst", cv2.WINDOW_NORMAL)
    cv2.imshow("src",image)
    cv2.imshow("dst",image_new)
    cv2.waitKey(0)



def checkerror():
    
    pass

if __name__ == "__main__":
    parse = argparse.ArgumentParser()
    parse.add_argument("-so","--saveoption",default= False,action="store_true",help = "是否保存为xml文件")
    parse.add_argument("-sp","--save_path",default= "./param/red.yaml",help="内参保存文件名")
    parse.add_argument("-er","--extrinsic_file_r",default= "r.txt",help="外参旋转保存文件名")
    parse.add_argument("-et","--extrinsic_file_t",default= "t.txt",help="外参平移保存文件名")
    parse.add_argument("-r","--root_dir",default= "E:\\dataset\\calibrate\\rgb+infrared\\rgbdslam\\rgb",help="标定图像根目录")
    # 棋盘格和参数
    parse.add_argument("-bt","--board_type",default="Corner",choices=["Corner","Circle"],help="棋盘格标定类型(角点、圆点)")
    parse.add_argument("-bs","--board_size",default= (7,6),type = int,nargs=2,help="棋盘格规格")
    parse.add_argument("-s","--square_size",default= 2 ,type= float,help="棋盘格尺寸")
    parse.add_argument("-rs","--radius_size",default=(5,5),type = int,nargs=2,help = "亚像素角点查找半径")
    parse.add_argument("-sd","--showdetail",default=False,action="store_true",help="是否展示具体内容")
    

    parse.add_argument("--resize_option",default=False,action="store_true",help="resize times")
    parse.add_argument("--resize_times",default=0.5,type=float,help="resize times")
    parse.add_argument("--img_width",default=640,type=int,help = "image width")
    parse.add_argument("--img_height",default=480,type=int,help = "image width")
    args=parse.parse_args()
    
    main(args)






