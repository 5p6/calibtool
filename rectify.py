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
    parser.add_argument("--config","-c",default="config/Fisheye/mono_corner.yaml")
    parser.add_argument("--left_img","-l",default="data/corner/left/1.jpg")
    parser.add_argument("--right_img","-r",default="data/corner/right/1.jpg")

    args = parser.parse_args()
    main(args)