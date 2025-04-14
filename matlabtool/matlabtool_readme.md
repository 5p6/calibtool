### 1.说明
由于有时候opencv的角点检测过于差，并且又因为本库支持角点、世界点导出csv文件，以及csv读入，所以这里用matlab检测角点。opencv算法优化就有了。

附注: ${data} 表示 data 的值，是一种泛化的表示。


### 2.使用方法
#### 2.1 matlab角点检测
首先你要有一个项目目录，如下
* 单目
```txt
- ${datadir}
    - calib
        - xxxx1.png
        - xxxx2.png
        - ...
```
* 双目
```txt
- ${datadir}
    - calib
        - left
            - xxxx1.png
            - xxxx2.png
            - ...
        - right
            - xxxx1.png
            - xxxx2.png
            - ...
```

请注意双目和单目的区别，双目`calib`文件夹下还有两个文件夹`left`、`right`，而单目`calib`文件夹下只有图像。
* 对于单目，修改`Main_MonoCalib.m`中代码，
```matlab
close all;clc;clear all;
% 参数
root_dir = '${datadir}/calib'; % 图像目录的路径
squareSizeInMM = 20; % 圆格：两个圆之间的距离(mm)，方格：方格的边长
board_type = "Circle"; % 类型，支持Corner和Circle
board_size = [7,7];% 棋盘格大小，圆格才使用
output_dir = "${output_dir}";% 输出路径
```
输出格式为
```txt
- ${output_dir}
    - corners
        - xxxx1.csv
        - xxxx2.csv
        - ...
    - world_coordinates.csv
```


* 对于双目，修改`Main_StereoCalib.m`中代码，
```matlab
% 参数
root_dir = '${datadir}/calib'; % left、right根目录的路径
squareSizeInMM = 20; % 圆格：两个圆之间的距离(mm)，方格：方格的边长
board_type = "Circle"; % 类型，支持Corner和Circle
board_size = [7, 7];% 棋盘格大小，圆格才使用
output_dir = "${output_dir}";% 输出路径
```
输出格式为
```txt
- ${output_dir}
    - left_corners
        - xxxx1.csv
        - xxxx2.csv
        - ...
    - right_corners
        - xxxx1.csv
        - xxxx2.csv
        - ...
    - world_coordinates.csv
```


### 3.OpenCV优化和保存
创建一个`config.yaml`在目录下
```txt
- calibtool
    - src
    - data
    - config.yaml
    - main.py
    - rectify.py
    - ...
```
* 单目
设置`config.yaml`的内容如下
```yaml
root_dir: ${datadir}/calib      # 一定要用!!!!!;单目标定图像的目录,目录下就是图像;
Camera_SensorType: Pinhole      # 支持: Pinhole, Fisheye, Omnidir
Camera_NumType: Monocular       # 一定要用!!!!!支持: Monocular, Stereo
square_size: ${squareSizeInMM}$               # 单位距离，比如每格边长 m
board_size: [7, 6]              # 棋盘格规格: 行数7, 列数6
board_type: Corner              # 支持: Corner 或 Circle
raduis_size: [5, 5] 
criteria: [3, 100, 1e-7]        # 终止条件: cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER
flag: 0                         # 标定标志位（opencv 标志组合）
save_dir: ${output_dir}
#  CSV数据加载选项
use_csv_data: true            # 是否使用CSV数据而非从图像检测角点，运行完false后可以设置为true试一下
image_points_dir: ${output_dir}       # 单目：图像角点CSV文件目录，会在该文件夹下创建一个corners文件，之后保存在output/corners里。
world_points_file: ${output_dir}/world_coordinates.csv # 世界坐标点CSV文件
```
运行
```bash
python3 main.py -c config.yaml
```


* 双目
设置`config.yaml`的内容如下
```yaml
root_dir: ${datadir}/calib      # 一定要用!!!!!;双目标定图像的目录,目录下是left、right目录;
Camera_SensorType: Pinhole      # 支持: Pinhole, Fisheye, Omnidir
Camera_NumType: Stereo       # 一定要用!!!!!支持: Monocular, Stereo
square_size: ${squareSizeInMM}               # 单位距离，比如每格边长 m
board_size: [7, 6]              # 棋盘格规格: 行数7, 列数6
board_type: Corner              # 支持: Corner 或 Circle
raduis_size: [5, 5] 
criteria: [3, 100, 1e-7]        # 终止条件: cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER
flag: 0                         # 标定标志位（opencv 标志组合）
save_dir: ${output_dir}
#  CSV数据加载选项
use_csv_data: true            # 是否使用CSV数据而非从图像检测角点，运行完false后可以设置为true试一下
image_points_dir: ${output_dir}       # 双目保存在 ${output_dir}/left 和 ${output_dir}/right目录下
world_points_file: ${output_dir}/world_coordinates.csv # 世界坐标点CSV文件
```
运行
```bash
python3 main.py -c config.yaml
```

* 检测显示
```bash
python3 rectify.py -c config.yaml
```