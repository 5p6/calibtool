root_dir=/mnt/d/dataset/CameraCalib/stereoexample_github/stereoexample_zed/calib/1920new/left
output_dir=./output
nx=4
ny=3
build/monodet --input-dir ${root_dir} --output-dir ${output_dir} --nx ${nx} --ny ${ny} --verbose