root_dir=/mnt/d/dataset/CameraCalib/stereoexample_github/stereoexample_zed/calib/1920new
output_dir=./output
nx=4
ny=3

build/stereodet --input-dir ${root_dir} --output-dir ${output_dir} --nx ${nx} --ny ${ny} --verbose