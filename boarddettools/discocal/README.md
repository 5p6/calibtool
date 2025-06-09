### 1.说明
由于有时候opencv的角点检测过于差，并且又因为本库支持角点、世界点导出csv文件，以及csv读入，所以这里用discocal检测圆心。

附注: ${data} 表示 data 的值，是一种泛化的表示。

### 2.环境编译
* WSL Ubuntu 20.04
* i5 12600

该环境依赖 
* OpenCV
* Eigen
* openmp

你可以用命令编译
```bash
cmake -S . -B build
cmake --build ./build -j 4
```
或者
```bash
mkdir build && cd build
cmake ..
make -j 4
```


### 3.使用方法
#### 3.1 单目
首先你要有一个项目目录，如下单目目录
```txt
- ${datadir}
    - calib
        - xxxx1.png
        - xxxx2.png
        - ...
```
之后修改 `demomono.sh`
```bash
root_dir=${datadir}/calib
output_dir=./output
nx=3
ny=4
./build/main_detect ${root_dir} ${output_dir} ${nx} ${ny}
```
其中
* root_dir    标定目录
* output_dir  输出目录
* nx          横轴圆心数
* ny          纵轴圆心数

之后运行
```bash
bash demomono.sh
```
效果如下图
![单目](../../document/monocircle.png)


#### 3.2 双目
首先你要有一个项目目录，如下双目目录
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
之后修改 `demo.sh`
```bash
root_dir=${datadir}/calib
output_dir=./output
nx=3
ny=4
./build/main_detect ${root_dir} ${output_dir} ${nx} ${ny}
```
其中
* root_dir    标定目录
* output_dir  输出目录
* nx          横轴圆心数
* ny          纵轴圆心数

之后运行
```bash
bash demostereo.sh
```
效果如下图
![单目](../../document/stereocircle.png)