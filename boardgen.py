import cv2
import numpy as np
import argparse
import os

def generate_circle_grid(rows, cols, circle_diameter, spacing, margin=20, output_path='circle_grid.png', dpi=300):
    mm_to_pixel = dpi / 25.4
    diameter_px = int(circle_diameter * mm_to_pixel)
    spacing_px = int(spacing * mm_to_pixel)

    margin_px = margin
    board_width = (cols - 1) * spacing_px + diameter_px + 2 * margin_px
    board_height = (rows - 1) * spacing_px + diameter_px + 2 * margin_px

    board = np.zeros((board_height, board_width), dtype=np.uint8)

    for i in range(rows):
        for j in range(cols):
            radius = diameter_px // 2
            center_x = margin_px + j * spacing_px + radius
            center_y = margin_px + i * spacing_px + radius
            center = (center_x, center_y)
            
            cv2.circle(board, center, radius, 255, -1, lineType=cv2.LINE_AA)

    # os.makedirs(os.path.dirname(output_path), exist_ok=True)
    cv2.imwrite(output_path, cv2.bitwise_not(board))
    print(f"[INFO] 圆形标定板已保存至：{output_path}")


def main():
    parser = argparse.ArgumentParser(description="生成圆形标定板图像 (Circle Grid Calibration Board)")
    parser.add_argument('--rows', type=int, default=4, help='圆点的行数（垂直方向）')
    parser.add_argument('--cols', type=int, default=3, help='圆点的列数（水平方向）')
    parser.add_argument('--diameter', type=float, default=60, help='每个圆的直径（单位：mm）')
    parser.add_argument('--spacing', type=float, default=70, help='圆心之间的间距（单位：mm）')
    parser.add_argument("--margin-pix",type=int,default=40,help="边缘的像素点")
    parser.add_argument('--dpi', type=int, default=300, help='图像DPI，用于控制分辨率（默认：300）')
    parser.add_argument('--output', type=str, default='circle_grid.png', help='输出图像路径（默认：circle_grid.png）')

    args = parser.parse_args()

    generate_circle_grid(
        rows=args.rows,
        cols=args.cols,
        circle_diameter=args.diameter,
        spacing=args.spacing,
        margin=args.margin_pix,
        output_path=args.output,
        dpi=args.dpi
    )

if __name__ == "__main__":
    main()
