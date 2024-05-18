### <div align="center">项目介绍</div>
这是一个用于棋盘格标定的算法库，适用于方棋盘格标定和圆棋盘格标定，同时为Python和C++两种语言提供了标定功能。

### <div align="center">项目环境</div>
* Ubunutu 20.04
* gnu 9.4.0

####  C++ 附加依赖
* OpenCV 4.5.5
* Boost

####  Python 附加依赖
* opencv-python 4.x
* numpy 1.20.x
* argparse
* tqmd

### <div align="center"> 系统构建</div>
Python直接使用即可，C++需要使用`sh`文件
```shell
cd ${yourproject}/calibC++
sh build.sh
```

### <div align="center">使用方法</div>
#### C++
这里只介绍最新版本的用法，旧版本详情请看[calibC++/README.md](calibC++/README.md)。


**单目**

使用如下命令行
```shell
./calibC++/Example/monocalib -r ${root_dir} -h ${board_height} -w ${board_width} -o ${output_dir} -s ${squre_size} -v
# for example
./calibC++/Example/monocalib -r ../data/left/ -h 6 -w 9 -o ./param -s 20 -v
```
你可以使用`--help`选项来查看帮助信息
```shell
./calibC++/Example/monocalib --help
```
**双目**

如下命令行
```shell
./calibC++/Example/stereocalib -l ${left_dir} -r ${right_dir} -h ${board_height} -w ${board_width} -s ${squre_size} -o ${output_dir} -v
# for example
./calibC++/Example/stereocalib -l ../data/left/ -r ../data/right/ -h 6 -w 9 -s 20 -o ./param -v
```
你同样可以使用`--help`选项来查看帮助信息
```shell
./calibC++/Example/stereocalib --help
```
**校正**
```shell
# monocular
./calibC++/Example/rectify -l ../data/left/1.jpg -p ./param/mono.yaml
# stereo
./calibC++/Example/rectify -l ../data/left/1.jpg -r ../data/right/1.jpg  -p ./param/stereo.yaml 
```



#### Python
**单目**
```shell
# 针孔相机模型
python calibpython/monocalib.py -r ${root_dir} -bs ${nx} ${ny} -s ${distance} -rs ${rx} ${ry}
# 鱼眼相机模型
python calibpython/monocalib-fisheye.py -r ${root_dir} -bs ${nx} ${ny} -s ${distance} -rs ${rx} ${ry}
# 全向相机模型
python calibpython/monocalib-omnidir.py -r ${root_dir} -bs ${nx} ${ny} -s ${distance} -rs ${rx} ${ry}
```
如果你需要其他的一些功能，可以添加`-h`选项，查看可选参数
```shell
python calibpython/monocalib-{camera_sensortype}.py -h
```
使用图像数据集目录在 `./data` ，棋盘格规格`6 X 9`,亚像素角点查找半径为`7 X 7 `，棋盘格尺寸为 $20$ mm ,则
```shell
# 使用示例数据
python calibpython/monocalib.py -r ./data/left -bs 6 9 -s 20 -rs 7 7 -so -sp param/mono.yaml -sd
```
**双目**
```shell
# 针孔相机模型
python calibpython/stereocalib.py -l ${left_root_dir} -r ${right_root_dir} -bs ${nx} ${ny} -s ${distance} -rs ${rx} ${ry}
# 鱼眼相机模型
python calibpython/stereocalib-fisheye.py -l ${left_root_dir} -r ${right_root_dir} -bs ${nx} ${ny} -s ${distance} -rs ${rx} ${ry}
# 全向相机模型
python calibpython/stereocalib-omnidir.py -l ${left_root_dir} -r ${right_root_dir} -bs ${nx} ${ny} -s ${distance} -rs ${rx} ${ry}
```
同样的可以选择附加选项 `-h` ，
```shell
python calibpython/stereocalib.py -h
```

使用示例数据
```shell
# 针孔相机模型
python calibpython/stereocalib.py -l ./data/left -r ./data/right -bs 6 9 -s 20 -rs 13 13 
```