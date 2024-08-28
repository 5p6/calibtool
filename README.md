### 1.简介
本次的代码主要用于相机的标定，其中包括
* 单目:针孔相机，鱼眼相机
* 双目:针孔相机，鱼眼相机

在运行时有许多参数可以使用，下述参数如果没有特殊声明就是可选项，对于单目通常是:
* root_dir 棋盘格标定图像的根目录(**必改**)
* board_size 棋盘格角点尺寸，是个整型的列表(**必改**)
* square_size 棋盘格小格子的尺寸，单位mm(**必改**)
* radius_size 亚角点查找半径
* showdetail 是否展示更长时间的角点查找图片
* saveoption 是否将计算出的参数保存
* save_path 参数保存的路径

对于双目通常是:
* left/right_root_dir 左/右目棋盘格标定图像的根目录(**必改**)
* board_size 棋盘格角点尺寸，是个整型的列表(**必改**)
* square_size 棋盘格小格子的尺寸，单位mm(**必改**)
* radius_size 亚角点查找半径
* showdetail 是否展示更长时间的角点查找图片
* saveoption 是否将计算出的参数保存
* save_path 参数保存的路径
* test_index 用于测试去畸变的图像索引
* save_img 是否保存去畸变的图像
* img_resizeoption 是否将图像重定尺寸(为了后续的立体匹配)
* resize_times 尺度倍数


单目和双目都会被保存的的参数有:
* Camera_type 相机的类型,["Pinhole","Fisheye"]
* height 图像的高
* width 图像的宽

单目额外保存的参数有:
* K 内参矩阵
* D 畸变系数


双目额外保存的参数有
* K_l 左相机内参矩阵
* D_l 左相机畸变系数
* K_r 右相机内参矩阵
* D_r 右相机畸变系数
* R 左目到右目的旋转矩阵
* T 左目到右目的平移向量

### 2.使用

直接终端运行即可
```shell
## 单目
python monocalib.py
## 双目
python stereocalib.py
```
可以使用 `-h` 选项查看命令行参数
```shell
## 单目
python monocalib.py -h
## 双目
python stereocalib.py -h
```

如果将 `saveoption` 设置为 `1` 那么你可以在指定文件夹中找到保存参数的文件。
