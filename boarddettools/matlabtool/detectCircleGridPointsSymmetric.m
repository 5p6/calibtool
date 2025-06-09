function [imagePoints,imagesUsed] = detectCircleGridPointsSymmetric(leftImages,rightImages, board_size)
numPairs = min(length(leftImages), length(rightImages));
% 初始化
imagePointsLeft = [];
% imagePointsRight = [];
imagesUsed = false(numPairs, 1);

validPairIndex = 0;

for i = 1:numPairs
    try
        [pointsLeft,boardSizeL] = detectCircleGridPoints(leftImages{i}, board_size,'PatternType','symmetric');
        [pointsRight, boardSizeR] = detectCircleGridPoints(rightImages{i}, board_size, 'PatternType', 'symmetric');

        if boardSizeL && boardSizeR
            validPairIndex = validPairIndex + 1;

            % 存储角点，格式 h x w x n x m（m=1 左, m=2 右）
            imagePointsLeft(:, :, validPairIndex) = pointsLeft;
            imagePointsRight(:, :, validPairIndex) = pointsRight;

            figure;
            subplot(1,2,1);
            imshow(imread(leftImages{i})); hold on;
            plot(pointsLeft(:,1), pointsLeft(:,2), 'go-');  % 绿点+绿线

            % 显示右图像角点与重投影点
            subplot(1,2,2);
            imshow(imread(rightImages{i})); hold on;
            plot(pointsRight(:,1), pointsRight(:,2), 'go-');  % 绿点+绿线



            imagesUsed(validPairIndex) = true;
        end
    catch
        % 如果有异常，跳过该对
        continue;
    end
end

% % 整合成 h * w x 2 x n x 2 的 imagePoints
h = board_size(2);  % 行（纵向点数）
w = board_size(1);  % 列（横向点数）
%
imagePoints = zeros(h * w , 2, validPairIndex, 2);  % 最后维是左右目
for i = 1:validPairIndex
    imagePoints(:,:, i, 1) = imagePointsLeft(:, :, i);  % 左
    imagePoints(:,:, i, 2) = imagePointsRight(:, :, i); % 右
end
end