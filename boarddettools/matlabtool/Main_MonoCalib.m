close all;clc;clear all;
% 参数
root_dir = 'E:\dataset\CameraCalib\public\circle\left';
squareSizeInMM = 20;
board_type = "Circle";
board_size = [7,7];
output_dir = "./output";



mkdir(fullfile(output_dir,"corners"));
% 标定
images = imageSet(root_dir);
imageFileNames = images.ImageLocation;

% 角点定位
if strcmp(board_type,"Corner")
    [imagePoints, board_size, imagesUsed] = detectCheckerboardPoints(imageFileNames);
    % 世界点
    worldPoints = generateCheckerboardPoints(board_size,squareSizeInMM);
elseif strcmp(board_type,"Circle")
    [imagePoints,imagesUsed] = detectCircleGridPoints(imageFileNames, board_size,...
        'PatternType','symmetric');
    % 世界点
    worldPoints = generateCircleGridPoints(board_size,squareSizeInMM,...
        'PatternType','symmetric');
end

% 标定算法
cameraParams = estimateCameraParameters(imagePoints, worldPoints, ...
    'WorldUnits', 'millimeters', 'EstimateSkew', true, 'EstimateTangentialDistortion', true);

% 显示标定结果
figure;
showReprojectionErrors(cameraParams);
title('重投影误差');

figure;
showExtrinsics(cameraParams);
title('相机姿态');


figure;
index  = 4;

drawCornerInOrder(imread(imageFileNames{index}), ...
    imagePoints(:,:,index,1), ...
    cameraParams.ReprojectedPoints(:,:,index), ...
    sprintf('Left Image #%d', index));

% 保存相机参数
save('cameraParameters.mat', 'cameraParams');

fprintf('相机标定完成，参数已保存至 cameraParameters.mat\n');

% 保存角点和世界点
index = 0;
% 保存角点信息到CSV文件
for i = 1:numel(imageFileNames)
    if ~imagesUsed(i,1)
        continue
    end
    index = index + 1;
    [~, baseFileName, ~] = fileparts(imageFileNames(index));
    csvFileName = fullfile(output_dir,"corners", [baseFileName '.csv']);
    % 取出当前图像的角点坐标
    currentImagePoints = imagePoints(:,:,index);
    % 拼接标题与数据
    T = array2table(currentImagePoints, 'VariableNames', {'image_x', 'image_y'});
    % 写入 CSV 文件
    writetable(T, csvFileName, 'Delimiter', ' ', 'WriteVariableNames', true);
    fprintf('保存图像 %s 的角点信息至 %s\n', baseFileName, csvFileName);
end

% 保存世界点信息到单独的CSV文件
worldPointsFileName = fullfile(output_dir, 'world_coordinates.csv');
worldPoints(:,3) = 0;
T = array2table(worldPoints, 'VariableNames', {'world_x', 'world_y', 'world_z'});
writetable(T, worldPointsFileName, 'Delimiter', ' ', 'WriteVariableNames', true);
fprintf('保存世界点信息至 %s\n', worldPointsFileName);
