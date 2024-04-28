clc; clear; close all;

%%%%%%%%%%%%%%% Translation Diff Plotting - Pos %%%%%%%%%%%%%%

for testNr = 0 : 4
    if testNr ~= 3
        fileName = ['ExportedResults_', num2str(testNr), '.csv'];
        data = csvread(fileName, 1,0);
        if testNr == 0
            time = data(:,1);
            xQTM = data(:,2);
            yQTM = data(:,3);
            zQTM = data(:,4);
            xCV0 = data(:,5);
            yCV0 = -data(:,6); % Flip axis, different frames
            zCV0 = data(:,7);
        else
            time = [time; data(:,1)];
            xQTM = [xQTM; data(:,2)];
            yQTM = [yQTM; data(:,3)];
            zQTM = [zQTM; data(:,4)];
            xCV0 = [xCV0; data(:,5)];
            yCV0 = [yCV0; -data(:,6)]; % Flip axis, different frames
            zCV0 = [zCV0; data(:,7)];
            % xCV1 = data(:,35)];
            % yCV1 = -data(:,36)]; % Same here
            % zCV1 = data(:,37)];
        end
    end
end
trans = [0.190 0.143 1.564]; % Physically measured in m

transDiffPlot = figure(Name='TransDiffPlot2');
sgtitle('Difference between transVector[0] and QTM as function of QTM')
dLims = 0.3;

% X plotting
transIndicesX = (xQTM ~= trans(1));
subplot(3,1,1)
xDiff = xQTM(transIndicesX) - xCV0(transIndicesX);
plot(xQTM(transIndicesX),xDiff, '.r')
ylabel('difference [m]')
xlabel('x [m]')
% xlim([startTime stopTime])
ylim([-dLims dLims])

% Y plotting - Remember to flip sign, only axis which is aligned
transIndicesY = (yQTM ~= trans(2));
subplot(3,1,2)
yDiff = yQTM(transIndicesY) - yCV0(transIndicesY);
plot(yQTM(transIndicesY),yDiff, '.g')
ylabel('difference [m]')
xlabel('y [m]')
% xlim([startTime stopTime])
ylim([-dLims dLims])

% Z plotting
transIndicesZ = (zQTM ~= trans(3));
subplot(3,1,3)
zDiff = zQTM(transIndicesZ) - zCV0(transIndicesZ);
plot(zQTM(transIndicesZ),zDiff, '.', 'Color',[109/255, 209/255, 255/255])
ylabel('difference [m]')
xlabel('z [m]')
% xlim([startTime stopTime])
ylim([-dLims dLims])

%% Extract Intervals

zQTM = zQTM(transIndicesZ);

% Split the data into evenly distributed intervals
distNums = 9;
intervals = linspace(0.10, 1.55, (distNums+1));

% Sort the lists to ascending order
[zQTMsorted, zQTMindices] = sort(zQTM);
zDiffSorted = zDiff(zQTMindices);

% Find indices closest to bounds of intervals
[~, closestIdx] = min( abs(zQTMsorted - intervals));

% Make cell to export lists to
intervalLists = cell(distNums,1,2);

intervalPlots = figure(Name="intervalPlots");
sgtitle('Interval Plots of z-axis')
for i = 1 : distNums
    % Save current interval values
    curQTM = zQTMsorted( closestIdx(i):closestIdx(i+1) );
    curDiff = zDiffSorted( closestIdx(i):closestIdx(i+1) );

    % Plot intervals
    subplot(3,3,i)
    plot(curQTM, curDiff,'.', 'Color',[109/255, 209/255, 255/255])
    ylabel('difference [m]')
    xlabel('z [m]')
    ylim([-dLims dLims])

    % Export data
    intervalLists{i,1,1} = curQTM;
    intervalLists{i,1,2} = curDiff;

    % Export intervals - UiO gpt tip
    % curQTMname = ['zQTMs_', num2str(distNums)];
    % curDiffName = ['zDiff_', num2str(distNums)];
    % eval([curQTMname, '= curQTM;'])
    % eval([curDiffName, '= curDiff;'])
end

pd = fitdist(intervalLists{1,1,2}, 'normal')

% tolerance = 0.01;  % [m]
% for i = 1 : length(intervals)
    % closest = zQTM(idx)
    
    % closestIdx = find(zQTM == closest)
% end

% for i = 1 : distNums
%     intervalIndices = (zQTMsorted >= intervals(i)) & (zQTM < intervals(i+1));
%     figure
%     plot(zQTM)
% 
% end



% for i = 1 : length(intervals)
%     if zDiff(i) < intervals
% end

% Export figure
%set(transPlot,'units','normalized','outerposition',[0 0 1 1])
% saveas(transDiffPlot, ['poseTest_',num2str(testNr), '_transDiffPlot'])
%saveas(transPlot, ['poseTest_',num2str(testNr), '_TransPlot.pdf'])
% exportgraphics(transDiffPlot, ['poseTest_',num2str(testNr), '_transDiffPlot.pdf'], ...
%                'ContentType', 'vector');
%%%%%%%%%%%%%%% Translation Diff Plotting - Pos %%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%% Translation Analysis %%%%%%%%%%%%%%%%%%%%
% 
% % Initialize difference lists
% diffxV01 = zeros(length(time), 1);
% diffyV01 = zeros(length(time), 1);
% diffzV01 = zeros(length(time), 1);
% diffxCVQTM = zeros(length(time), 1);
% diffyCVQTM = zeros(length(time), 1);
% diffzCVQTM = zeros(length(time), 1);
% % Start times
% timeTol = 1e-3;
% zStartTime = 192.379;
% zStopTime = 243.111;
% zStartAvgDiff = find(abs(time-zStartTime) < timeTol);
% zStopAvgDiff = find(abs(time-zStopTime) < timeTol);
% % Append differences to lists
% for i = 1 : length(time)
%     if (xQTM(i) ~= trans(1)) && (xCV0(i) ~= 0)
%         diffxV01(i) = abs(xCV0(i) - xCV1(i));
%         diffxCVQTM(i) = abs(xCV0(i) - xQTM(i));
%     end
%     if (yQTM(i) ~= trans(2)) && (yCV0(i) ~= 0)
%         diffyV01(i) = abs(yCV0(i) - yCV1(i));
%         diffyCVQTM(i) = abs(yCV0(i) - yQTM(i)); % Sign flip, diff frames
%     end
% end
% for i = zStartAvgDiff : zStopAvgDiff
%     if (zQTM(i) ~= trans(3)) && (zCV0(i) ~= 0)
%         diffzV01(i) = abs(zCV0(i) - zCV1(i));
%         diffzCVQTM(i) = abs(zCV0(i) - zQTM(i));
%     end
% end
% 
% format long
% % Calculate average difference from list
% avgDiffxVecs = mean(diffxV01);
% avgDiffyVecs = mean(diffyV01);
% avgDiffzVecs = mean(diffzV01);
% avgDiffxCVQTM = mean(diffxCVQTM);
% avgDiffyCVQTM = mean(diffyCVQTM);
% avgDiffzCVQTM = mean(diffzCVQTM);
% % Present average differences
% avgDiffMeters = table(avgDiffxVecs, avgDiffyVecs, avgDiffzVecs, ...
%                      avgDiffxCVQTM, avgDiffyCVQTM, avgDiffzCVQTM)
% %%%%%%%%%%%%%%%%%%%% Translation Analysis %%%%%%%%%%%%%%%%%%%%
% 
% % startTime = 140;
% % stopTime = 186;
% startTime = 0;
% stopTime = time(end);
% 
% %%%%%%%%%%%%%%%%%%%% Translation Plotting %%%%%%%%%%%%%%%%%%%%
% transPlot = figure(Name="Plot of Translation comparison");
% 
% % X plotting
% transIndicesX = (xQTM ~= trans(1));
% sgtitle("Position Comparison: transVector[0] and QTM")
% subplot(3,1,1)
% plot(time(transIndicesX),xCV0(transIndicesX), '.r')
% hold on
% plot(time(transIndicesX),xQTM(transIndicesX), '.k', MarkerSize=1)
% ylabel('x [m]')
% xlabel('Time [seconds]')
% % xlim([0 time(end)])
% xlim([startTime stopTime])
% [a, icons] = legend('xCV', 'xQTM', 'Location','eastoutside');
% % Change size of legend icons
% icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
% set(icons, 'MarkerSize', 20)
% 
% % Y plotting - Remember to flip sign, only axis which is aligned
% transIndicesY = (yQTM ~= trans(2));
% subplot(3,1,2)
% plot(time(transIndicesY),yCV0(transIndicesY), '.g')
% hold on
% plot(time(transIndicesY),yQTM(transIndicesY), '.k', MarkerSize=1)
% ylabel('y [m]')
% xlabel('Time [seconds]')
% % xlim([0 time(end)])
% xlim([startTime stopTime])
% [a, icons] = legend('yCV0', 'yQTM', 'Location','eastoutside');
% % Change size of legend icons
% icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
% set(icons, 'MarkerSize', 20)
% 
% % Z plotting
% transIndicesZ = (zQTM ~= trans(3));
% subplot(3,1,3)
% plot(time(transIndicesZ),zCV0(transIndicesZ), ...
%     '.', 'Color',[109/255, 209/255, 255/255])
% hold on
% plot(time(transIndicesZ),zQTM(transIndicesZ), '.k', MarkerSize=1)
% ylabel('z [m]')
% xlabel('Time [seconds]')
% % xlim([0 time(end)])
% xlim([startTime stopTime])
% [a, icons] = legend('zCV0', 'zQTM', 'Location','eastoutside');
% % Change size of legend icons
% icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
% set(icons, 'MarkerSize', 20)
% 
% % Export figure
% %set(transPlot,'units','normalized','outerposition',[0 0 1 1])
% saveas(transPlot, ['poseTest_',num2str(testNr), '_TransPlot'])
% %saveas(transPlot, ['poseTest_',num2str(testNr), '_TransPlot.pdf'])
% exportgraphics(transPlot, ['poseTest_',num2str(testNr), '_TransPlot.pdf'], ...
%                'ContentType', 'vector');
% %%%%%%%%%%%%%%%%%%%% Translation Plotting %%%%%%%%%%%%%%%%%%%%
% 
% %%%%%%%%%%%%% Temporal Translation Diff Plotting %%%%%%%%%%%%%
% transDiffTemporalPlot = figure(Name='TransDiffPlot');
% sgtitle('Difference between transVector[0] and QTM as function of time')
% dLims = 0.3;
% 
% % X plotting
% transIndicesX = (xQTM ~= trans(1));
% subplot(3,1,1)
% xDiff = xQTM(transIndicesX) - xCV0(transIndicesX);
% plot(time(transIndicesX),xDiff, '.r')
% ylabel('x [m]')
% xlabel('Time [seconds]')
% % xlim([0 time(end)])
% xlim([startTime stopTime])
% ylim([-dLims dLims])
% %[a, icons] = legend('Diff', 'Location','eastoutside');
% % Change size of legend icons
% %icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
% %set(icons, 'MarkerSize', 20)
% 
% % Y plotting - Remember to flip sign, only axis which is aligned
% transIndicesY = (yQTM ~= trans(2));
% subplot(3,1,2)
% yDiff = yQTM(transIndicesY) - yCV0(transIndicesY);
% plot(time(transIndicesY),yDiff, '.g')
% ylabel('y [m]')
% xlabel('Time [seconds]')
% % xlim([0 time(end)])
% xlim([startTime stopTime])
% ylim([-dLims dLims])
% 
% % Z plotting
% transIndicesZ = (zQTM ~= trans(3));
% subplot(3,1,3)
% zDiff = zQTM(transIndicesZ) - zCV0(transIndicesZ);
% plot(time(transIndicesZ),zDiff, '.', 'Color',[109/255, 209/255, 255/255])
% ylabel('z [m]')
% xlabel('Time [seconds]')
% % xlim([0 time(end)])
% xlim([startTime stopTime])
% ylim([-dLims dLims])
% 
% % Export figure
% %set(transPlot,'units','normalized','outerposition',[0 0 1 1])
% saveas(transDiffTemporalPlot, ['poseTest_',num2str(testNr), '_transDiffTemporalPlot'])
% %saveas(transPlot, ['poseTest_',num2str(testNr), '_TransPlot.pdf'])
% exportgraphics(transDiffTemporalPlot, ['poseTest_',num2str(testNr), '_transDiffTemporalPlot.pdf'], ...
%                'ContentType', 'vector');
% %%%%%%%%%%%%% Temporal Translation Diff Plotting %%%%%%%%%%%%%