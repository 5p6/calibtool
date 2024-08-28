### 1.项目简介
这是一个代码部分来自于开源库 [camodocal](https://github.com/hengli/camodocal) 的 基于 `ceres` 的相机标定项目，支持不同的相机类型和棋盘格类型，其中具体如下表

| 相机类型\棋盘格类型     | 方块           | 圆形           |
| -------------- | ------------ | ------------ |
| Mei            | $\checkmark$ | $\checkmark$ |
| Pinhole        | $\checkmark$ | $\checkmark$ |
| Kannala-brandt | $\checkmark$ | $\checkmark$ |

### 2.依赖
* OpenCV 3.x
* Boost
* Ceres
* Eigen 3.x

### 3.编译
当满足上述条件后，即可编译
```shell
$ cd {yourproject}
$ cmake -S . -B build
$ cmake --build build --config release -j
```
### 4.示例
在目录下已经存在多个不同摄像机模型和棋盘格类型的数据，在`./data`下，可以通过命令行读取解释
```shell
# 单目的帮助文档
$ ./build/monocalib --help
# 双目的帮助文档
$ ./build/stereocalib --help
```
#### 4.1 单目方格
```shell
$ cd {yourproject}
$ ./build/monocalib -w 9 -h 6 -s 120 -i ./data/mei/ -p img -e .bmp --camera-model Mei -v --view-results
```
#### 4.2 单目圆格
```shell
$ ./build/monocalib -w 7 -h 7 -s 20 -i ./data/pinhole/Circle/ -p left_ -e .jpg --camera-model Pinhole -v --view-results --board-type Circle --opencv
```

#### 4.3 双目方格
```shell
$ ./build/stereocalib -w 6 -h 9 -s 20 -i ./data/pinhole/Square --prefix-l left --prefix-r right -e .jpg --camera-model Pinhole -v --board-type Square --view-results
```
#### 4.4 双目圆格
```shell
$ ./build/stereocalib -w 7 -h 7 -s 20 -i ./data/pinhole/Circle --prefix-l left_ --prefix-r right_ -e .jpg --camera-model Pinhole -v --board-type Circle --view-results --opencv
```



### citation
```shell
Lionel Heng, Bo Li, and Marc Pollefeys, CamOdoCal: Automatic Intrinsic and
Extrinsic Calibration of a Rig with Multiple Generic Cameras and Odometry,
In Proc. IEEE/RSJ International Conference on Intelligent Robots
and Systems (IROS), 2013.
```