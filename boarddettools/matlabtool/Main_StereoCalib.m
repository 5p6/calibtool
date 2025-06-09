close all;clc;clear all;
% 参数
root_dir = 'E:\dataset\CameraCalib\StereoShip\newcalib\';
squareSizeInMM = 20;
board_type = "Corner";
board_size = [7, 7]; % yuan'ge
output_dir = "./output";

% 创建目录
mkdir(fullfile(output_dir,"left_corners"));
mkdir(fullfile(output_dir,"right_corners"));

% 左右图像目录的路径
leftImages = imageSet(fullfile(root_dir,'left')).ImageLocation;
rightImages = imageSet(fullfile(root_dir,'right')).ImageLocation;

% 检测角点
if strcmp(board_type,"Corner")
    [imagePoints,board_size,pairsUsed] = ...
        detectCheckerboardPoints(leftImages,rightImages);
    % 世界点yuange
    worldPoints = generateCheckerboardPoints(board_size,squareSizeInMM);
elseif strcmp(board_type,"Circle")
    [imagePoints, pairsUsed] = detectCircleGridPointsSymmetric(leftImages,rightImages, board_size);
    % 世界点
    worldPoints = generateCircleGridPoints(board_size,squareSizeInMM,...
        'PatternType','symmetric');
end


% 参数估计
cameraParams = estimateCameraParameters(imagePoints,worldPoints, ...
    'WorldUnits', 'millimeters', 'EstimateTangentialDistortion', true);

% 误差显示
figure;
showReprojectionErrors(cameraParams);

figure;
showExtrinsics(cameraParams);

% 显示 i 用于匹配图像 <-> index 用于确定使用的角点
count = length(pairsUsed);
index = 0;
for i = 1:count
    if ~pairsUsed(i,1)
        continue
    end
    index = index + 1;
    [~, baseFileName, ~] = fileparts(leftImages(index));
    figure;
    % 显示左图像角点与重投影点
    subplot(1,2,1);
    drawCornerInOrder(imread(leftImages{i}), ...
        imagePoints(:,:,index,1), ...
        cameraParams.CameraParameters1.ReprojectedPoints(:,:,index), ...
        sprintf('Left Image %s', baseFileName));


    % 显示右图像角点与重投影点
    subplot(1,2,2);
    drawCornerInOrder(imread(rightImages{i}), ...
        imagePoints(:,:,index,2), ...
        cameraParams.CameraParameters2.ReprojectedPoints(:,:,index), ...
        sprintf('Right Image %s', baseFileName));

end

% 保存相机参数
save('cameraParameters.mat', 'cameraParams');
fprintf('相机标定完成，参数已保存至 cameraParameters.mat\n');


% 保存角点和世界点
index = 0;
% 保存角点信息到CSV文件
for i = 1:numel(leftImages)
    if ~pairsUsed(i,1)
        continue
    end
    index = index + 1;
    [~, baseFileName, ~] = fileparts(leftImages(index));
    left_csvFileName = fullfile(output_dir,"left_corners", [baseFileName '.csv']);
    right_csvFileName = fullfile(output_dir,"right_corners", [baseFileName '.csv']);
    % 取出当前图像的角点坐标
    left_currentImagePoints = imagePoints(:,:,index,1);
    right_currentImagePoints = imagePoints(:,:,index,2);

    % 拼接标题与数据
    left_T = array2table(left_currentImagePoints, 'VariableNames', {'image_x', 'image_y'});
    right_T = array2table(right_currentImagePoints, 'VariableNames', {'image_x', 'image_y'});

    % 写入 CSV 文件
    writetable(left_T, left_csvFileName, 'Delimiter', ' ', 'WriteVariableNames', true);
    fprintf('保存图像 %s 的角点信息至 %s\n', baseFileName, left_csvFileName);
    writetable(right_T, right_csvFileName, 'Delimiter', ' ', 'WriteVariableNames', true);
    fprintf('保存图像 %s 的角点信息至 %s\n', baseFileName, right_csvFileName);

end

% 保存世界点信息到单独的CSV文件
worldPointsFileName = fullfile(output_dir, 'world_coordinates.csv');
worldPoints(:,3) = 0;
T = array2table(worldPoints, 'VariableNames', {'world_x', 'world_y', 'world_z'});
writetable(T, worldPointsFileName, 'Delimiter', ' ', 'WriteVariableNames', true);
fprintf('保存世界点信息至 %s\n', worldPointsFileName);

