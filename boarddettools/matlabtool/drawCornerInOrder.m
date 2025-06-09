function drawCornerInOrder(img, imagePoints, reprojectedPoints, titleText)
% 直接按照 imagePoints 的顺序连接线
% imagePoints 和 reprojectedPoints 应该是 N x 2

    imshow(img); hold on;

    % 角点顺序连接
    plot(imagePoints(:,1), imagePoints(:,2), 'go-');  % 绿点+绿线

    % 重投影点顺序连接
    plot(reprojectedPoints(:,1), reprojectedPoints(:,2), 'r+--');  % 红点+红虚线

    title(titleText);
    legend({'Detected Points & Path','Reprojected Points & Path','Error Vectors'}, ...
           'Location','bestoutside');
    hold off;
end