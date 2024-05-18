### 1.Introdction
This is a C++library for camera calibration. The types of camera sensors that can be calibrated include `pinhole` cameras and `fisheye` cameras, as well as `monocular` and `stereo` camera calibration. Based on this library, `RGBD` camera calibration and registration can be reopened
This library can be used for  `Linux` operating system.


### 2.Dependcies
* OpenCV 4.x
* Boost

### 3.Build
If the environment are linux,you need to change some varable in the `CMakeLists` ,which include the fmt and opencv path,the 
```shell
cd your_calib_path
```
and
```shell
cmake -S . -B ./build
cmake --build ./build --config release
```
or
```shell
sh build.sh
```

So you could make use of the executable file to calib

### 4.Example
The number of camera could be recongized by the code,so the only thing that you need to do is to check the calibration blank type and the camera typy,which involved:
*  calibration blank type
   *  CircleGrid 
   *  CornerGrid 
*  camera typy
   *  Pinhole
   *  Fisheye

then,you need to confirm the blank size(mXn) and square size(mm),just like this
#### 4.1 Monocular Camera
Assuming your data directory is as follows
```shell
- data
    -calib
        - img1.png
        - img2.png
        ...
```
then you need to confirm the parameters like
* board_width : assume `w`
* board_height : assume `h`
* squre_size : assume `l(mm)`
* Camera_SensorType : you can choose `[Pinhole,Fisheye]`
* Chessboard_Type : you can choose `[Corner,Circle]`
* Param_path : the file you save the parameter
##### old
Run using the command line
```shell
./Example_old/example_calib Mono ./data/calib ${board_width} ${board_height} ${squre_size} ${Camera_SensorType} ${Chessboard_Type} ${Param_path}
# for example
./Example_old/example_calib Mono ./data/calib m n l Pinhole Corner calibconfig.yaml
```

##### new
Run using the command line
```shell
./Example/monocalib -r ${root_dir} -h ${board_height} -w ${board_width} -o ${output_dir} -s ${squre_size} -v
# for example
./Example/monocalib -r ../data/left/ -h 6 -w 9 -o ./param -s 20 -v
```
you could use the `--help` option to check the help context,like
```shell
./Example/monocalib --help
```


#### 4.2 Stereo Camera
Assuming your data directory is as follows
```shell
- data
    - calib
        - left
            - img1.png
            - img2.png
            ...
        - right
            - img1.png
            - img2.png
            ...
```
then you need to confirm the parameters like
* board_width : assume `m`
* board_height : assume `n`
* squre_size : assume `l(mm)`
* Camera_SensorType : you can choose `[Pinhole,Fisheye]`
* Chessboard_Type : you can choose `[Corner,Circle]`
* Param_path : the file you save the parameter

Run using the command line
```shell
./Example_old/example_calib Stereo ../data/ ${board_width} ${board_height} ${squre_size} ${Camera_SensorType} ${Chessboard_Type} ${Param_path}
# for example
./Example_old/example_calib Stereo ../data/ m n l Pinhole Corner test.yaml
```

##### new
```shell
./Example/stereocalib -l ${left_dir} -r ${right_dir} -h ${board_height} -w ${board_width} -s ${squre_size} -o ${output_dir} -v
# for example
./Example/stereocalib -l ../data/left/ -r ../data/right/ -h 6 -w 9 -s 20 -o ./param -v
```
you could use the `--help` option to check the help context,like
```shell
./Example/stereocalib --help
```


#### 4.3 Rectify Image
Using program `example_rectify` ,you can rectify the Monocular/Stereo Image,like this
**old**
```shell
## Monocular
./Example_old/example_rectify calib.yaml ./data/img1.png 

## Stereo
./Example_old/example_rectify calib.yaml ./data/left/left1.png ./data/right/right1.png 
```
**new**
```shell
# monocular
./calibC++/Example/rectify -l ../data/left/1.jpg -p ./param/mono.yaml
# stereo
./calibC++/Example/rectify -l ../data/left/1.jpg -r ../data/right/1.jpg  -p ./param/stereo.yaml 
```