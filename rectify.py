from src import parse_yaml,Camera
import argparse
import cv2
from pathlib import Path
def main(args):
    config = parse_yaml(args.config)
    cam = Camera(config)
    # 误差
    error = cam.compute_reprojection_errors()
    
    # 显示
    left_img = cv2.imread(args.left_img)
    right_img = None
    if config.camera_num_type == "Stereo":
        right_img = cv2.imread(args.right_img)

    left,right,concat = cam.rectify(left_img,right_img)
    cv2.imshow("left",left)
    if config.camera_num_type == "Stereo":
        cv2.imshow("right",right)
        cv2.imshow("concat",concat)
    cv2.imwrite(Path(config.output_dir) / "concated_rectify.jpg",concat)
    cv2.waitKey()
    



if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--config","-c",default="stereo_ship.yaml")
    parser.add_argument("--left_img","-l",default="/mnt/e/dataset/CameraCalib/StereoShip/newcalib/left/1744186012028669503.jpg")
    parser.add_argument("--right_img","-r",default="/mnt/e/dataset/CameraCalib/StereoShip/newcalib/right/1744186012028669503.jpg")

    args = parser.parse_args()
    main(args)