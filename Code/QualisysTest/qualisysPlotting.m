clc; clear; close all;

%% Import Results from Pose Validation Test

testNr = 2;
fileName = ['ExportedResults_', num2str(testNr), '.csv'];
data = csvread(fileName, 1,0);

time = data(:,1);
xQTM = data(:,2);
yQTM = data(:,3);
zQTM = data(:,4);
xCV0 = data(:,5);
yCV0 = -data(:,6); % Flip axis
zCV0 = data(:,7);
xCV1 = data(:,35);
yCV1 = -data(:,36); % Flip axis
zCV1 = data(:,37);

trans = [0.190 0.143 1.564]; % Physically measured in m

% x Limits
startTime = 0;
stopTime  = time(end);
% For Translation Plots
startTimeTrans = 9.4;
stopTimeTrans  = 186;

%% Translation Analysis

% Initialize difference lists
diffxV01 = zeros(length(time), 1);
diffyV01 = zeros(length(time), 1);
diffzV01 = zeros(length(time), 1);
diffxCV0QTM = zeros(length(time), 1);
diffyCV0QTM = zeros(length(time), 1);
diffzCV0QTM = zeros(length(time), 1);
diffxCV1QTM = zeros(length(time), 1);
diffyCV1QTM = zeros(length(time), 1);
diffzCV1QTM = zeros(length(time), 1);

% Append differences to lists
for i = 1 : length(time)
    if (xQTM(i) ~= trans(1)) && (xCV0(i) ~= 0)
        diffxV01(i)    = abs(xCV0(i) - xCV1(i));
        diffxCV0QTM(i) = abs(xCV0(i) - xQTM(i));
        diffxCV1QTM(i) = abs(xCV1(i) - xQTM(i));
    end
    if (yQTM(i) ~= trans(2)) && (yCV0(i) ~= 0)
        diffyV01(i)    = abs(yCV0(i) - yCV1(i));
        diffyCV0QTM(i) = abs(yCV0(i) - yQTM(i)); % Sign flip, diff frames
        diffyCV1QTM(i) = abs(yCV1(i) - yQTM(i)); % Sign flip, diff frames
    end
    if (zQTM(i) ~= trans(3)) && (zCV0(i) ~= 0)
        diffzV01(i)    = abs(zCV0(i) - zCV1(i));
        diffzCV0QTM(i) = abs(zCV0(i) - zQTM(i));
        diffzCV1QTM(i) = abs(zCV1(i) - zQTM(i));
    end
end

format long
% Calculate average difference from list
avgDiffxVecs = mean(diffxV01);
avgDiffyVecs = mean(diffyV01);
avgDiffzVecs = mean(diffzV01);
avgDiffxCV0QTM = mean(diffxCV0QTM);
avgDiffyCV0QTM = mean(diffyCV0QTM);
avgDiffzCV0QTM = mean(diffzCV0QTM);
avgDiffxCV1QTM = mean(diffxCV1QTM);
avgDiffyCV1QTM = mean(diffyCV1QTM);
avgDiffzCV1QTM = mean(diffzCV1QTM);
% Present Mean Absolute Deviation
madVecs = table(avgDiffxVecs, avgDiffyVecs, avgDiffzVecs)
madQTMv0 = table(avgDiffxCV0QTM, avgDiffyCV0QTM, avgDiffzCV0QTM)
madQTMv1 = table(avgDiffxCV1QTM, avgDiffyCV1QTM, avgDiffzCV1QTM)

%% Translation Plotting
transPlot = figure(Name="Plot of Translation comparison");

% X plotting
transIndicesX = (xQTM ~= trans(1));
sgtitle("Position Comparison: transVectors[0] and QTM")
subplot(3,1,1)
plot(time,xCV0, '.r')
hold on
plot(time(transIndicesX),xQTM(transIndicesX), '.k', MarkerSize=1)
ylabel('x [m]')
xlabel('Time [seconds]')
xlim([startTimeTrans stopTimeTrans])
[~, icons] = legend('xCV', 'xQTM', 'Location','eastoutside');
% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

% Y plotting - Remember to flip sign, only axis which is aligned
transIndicesY = (yQTM ~= trans(2));
subplot(3,1,2)
plot(time,yCV0, '.g')
hold on
plot(time(transIndicesY),yQTM(transIndicesY), '.k', MarkerSize=1)
ylabel('y [m]')
xlabel('Time [seconds]')
xlim([startTimeTrans stopTimeTrans])
[~, icons] = legend('yCV0', 'yQTM', 'Location','eastoutside');
% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

% Z plotting
transIndicesZ = (zQTM ~= trans(3));
subplot(3,1,3)
plot(time,zCV0, ...
    '.', 'Color',[109/255, 209/255, 255/255])
hold on
plot(time(transIndicesZ),zQTM(transIndicesZ), '.k', MarkerSize=1)
ylabel('z [m]')
xlabel('Time [seconds]')
xlim([startTimeTrans stopTimeTrans])
[~, icons] = legend('zCV0', 'zQTM', 'Location','eastoutside');
% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

% Export figure
% set(transPlot,'units','normalized','outerposition',[0 0 1 1])
saveas(transPlot, ['poseTest_',num2str(testNr), '_TransPlot'])
saveas(transPlot, ['poseTest_',num2str(testNr), '_TransPlot.png'])

%% Translation Difference between QTM and transVectors[0]
transDiffPlot = figure(Name='TransDiffPlot');
sgtitle('Difference between transVectors[0] and QTM as function of QTM')
dLims = 0.3;

% X plotting
transIndicesX = (xQTM ~= trans(1));
subplot(3,1,1)
xDiff = xQTM(transIndicesX) - xCV0(transIndicesX);
plot(xQTM(transIndicesX),xDiff, '.r')
ylabel('difference [m]')
xlabel('x [m]')
ylim([-dLims dLims])

% Y plotting - Remember to flip sign, only axis which is aligned
transIndicesY = (yQTM ~= trans(2));
subplot(3,1,2)
yDiff = yQTM(transIndicesY) - yCV0(transIndicesY);
plot(yQTM(transIndicesY),yDiff, '.g')
ylabel('difference [m]')
xlabel('y [m]')
ylim([-dLims dLims])

% Z plotting
transIndicesZ = (zQTM ~= trans(3));
subplot(3,1,3)
zDiff = zQTM(transIndicesZ) - zCV0(transIndicesZ);
plot(zQTM(transIndicesZ),zDiff, '.', 'Color',[109/255, 209/255, 255/255])
ylabel('difference [m]')
xlabel('z [m]')
ylim([-dLims dLims])

% Export figure
saveas(transDiffPlot, ['poseTest_',num2str(testNr), '_transDiffPlot'])
saveas(transDiffPlot, ['poseTest_',num2str(testNr), '_TransDiffPlot.png'])

%% Rotation Comparison: rotMat0 and QTM
rot0matPlot = figure(Name="Rotation comparison0");
sgtitle("Rotation Matrix Comparison: rotMat0 and QTM")

% Plotting rotVectors[0] with QTM
for i = 1:9
    subplot(3, 3, i);
    
    vec0 = data(:, i+7); % v0Rij
    QTM = data(:, i+25); % v1Rij

    % Plot only when QTM is tracking
    validIndices = (QTM ~= 0);
    if any(i == [1,4,7])
        plot(time(validIndices), vec0(validIndices), '.r')
    elseif any(i == [2,5,8])
        plot(time(validIndices), vec0(validIndices), '.g')
    else
        plot(time(validIndices), vec0(validIndices), ...
            '.', 'Color',[109/255, 209/255, 255/255])
    end
    hold on
    q = plot(time(validIndices), QTM(validIndices), '.k', MarkerSize=1);
    
    xlabel('Time [seconds]');
    xlim([startTime stopTime])
    title(['Element ', num2str(i)])
    hold off
    ylim([-1 1])
end

% Add a bit space to the figure
fig = gcf;
fig.Position(3) = fig.Position(3) + 250;

% Add common legend outside subplots
lgdEntries = {'rVec0x','rVec0y','rVec0z', 'QTM'};
hold on
v1 = plot(nan, nan, '.r');
v2 = plot(nan, nan, '.g');
v3 = plot(nan, nan, '.', 'Color',[109/255, 209/255, 255/255]);
[lgd, icons] = legend([v1 v2 v3 q], lgdEntries);
lgd.Position(1) = 0.01;
lgd.Position(2) = 0.4;

% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

% Export figure
set(rot0matPlot,'units','normalized','outerposition',[0 0 1 1])
saveas(rot0matPlot, ['poseTest_',num2str(testNr), '_rot0matPlot'])
saveas(rot0matPlot, ['poseTest_',num2str(testNr), '_rot0matPlot.png'])

%% Rotation: Difference between rotMat0 and QTM
rot0matDiffPlot = figure(Name="RotationDifference0");
sgtitle("Rotation Matrix Element Difference: rotMat0 - QTM")

% Plotting rotVectors[0] - QTM
for i = 1:9
    subplot(3, 3, i);
    
    vec0 = data(:, i+7); % v0Rij
    QTM = data(:, i+25); % v1Rij
    
    % Plot only when QTM is tracking
    validIndices = (QTM ~= 0);
    plot(time(validIndices), (vec0(validIndices)-QTM(validIndices)), ...
         '.k', MarkerSize=1)
    
    xlabel('Time [seconds]');
    xlim([startTime stopTime])
    title(['Element ', num2str(i)])
    hold off
    ylim([-1 1])
end

% Export figure
set(rot0matDiffPlot,'units','normalized','outerposition',[0 0 1 1])
saveas(rot0matDiffPlot, ['poseTest_',num2str(testNr), '_rot0matDiffPlot'])
saveas(rot0matDiffPlot, ['poseTest_',num2str(testNr), '_rot0matDiffPlot.png'])

%% Rotation Comparison: rotMat1 and QTM
rot1matPlot = figure(Name="Rotation Comparison1");
sgtitle("Rotation Matrix Comparison: rotMat1 and QTM")

% Plotting rotVectors[1] with QTM
for i = 1:9
    subplot(3, 3, i);
    
    vec1 = data(:, i+16); % v0Rij
    QTM = data(:, i+25); % v1Rij
    
    % Plot only when QTM is tracking
    validIndices = (QTM ~= 0);
    if any(i == [1,4,7])
        plot(time(validIndices), vec1(validIndices), '.r')
    elseif any(i == [2,5,8])
        plot(time(validIndices), vec1(validIndices), '.g')
    else
        plot(time(validIndices), vec1(validIndices), ...
            '.', 'Color',[109/255, 209/255, 255/255])
    end
    hold on
    q = plot(time(validIndices), QTM(validIndices), '.k', MarkerSize=1);
    
    xlabel('Time [seconds]');
    xlim([startTime stopTime])
    title(['Element ', num2str(i)])
    hold off
    ylim([-1 1])
end

% Add a bit space to the figure
fig = gcf;
fig.Position(3) = fig.Position(3) + 250;

% Add common legend outside subplots
lgdEntries = {'rVec0x','rVec0y','rVec0z', 'QTM'};
hold on
v1 = plot(nan, nan, '.r');
v2 = plot(nan, nan, '.g');
v3 = plot(nan, nan, '.', 'Color',[109/255, 209/255, 255/255]);
[lgd, icons] = legend([v1 v2 v3 q], lgdEntries);
lgd.Position(1) = 0.01;
lgd.Position(2) = 0.4;

% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

% Export figure
set(rot1matPlot,'units','normalized','outerposition',[0 0 1 1])
saveas(rot1matPlot, ['poseTest_',num2str(testNr), '_rot1matPlot'])
saveas(rot1matPlot, ['poseTest_',num2str(testNr), '_rot1matPlot.png'])

%% Rotation: Difference between rotMat1 and QTM
rot1matDiffPlot = figure(Name="RotationDifference1");
sgtitle("Rotation Matrix Element Difference: rotMat1 - QTM")

% Plotting rotVectors[1] with QTM
for i = 1:9
    subplot(3, 3, i);
    
    vec1 = data(:, i+16); % v0Rij
    QTM = data(:, i+25); % v1Rij
    
    % Plot only when QTM is tracking
    validIndices = (QTM ~= 0);
    plot(time(validIndices), (vec1(validIndices)-QTM(validIndices)), ...
         '.k', MarkerSize=1)
    
    xlabel('Time [seconds]');
    xlim([startTime stopTime])
    title(['Element ', num2str(i)])
    hold off
    ylim([-1 1])
end

% Export figure
set(rot1matDiffPlot,'units','normalized','outerposition',[0 0 1 1])
saveas(rot1matDiffPlot, ['poseTest_',num2str(testNr), '_rot1matDiffPlot'])
saveas(rot1matDiffPlot, ['poseTest_',num2str(testNr), '_rot1matDiffPlot.png'])

%% Angle Difference: rotMat to Axis-Angle
angleDiffPlot = figure(Name="AngleDiffPlot");
hold on

% Initialize
angChoice = zeros(length(time), 1);

validIndices = [];

for i = 1 : length(time)
    % Extract Matrices
    rotMat0 = [data(i,8), data(i,9), data(i,10);
               data(i,11), data(i,12), data(i,13);
               data(i,14), data(i,15), data(i,16)];
    rotMat1 = [data(i,17), data(i,18), data(i,19);
               data(i,20), data(i,21), data(i,22);
               data(i,23), data(i,24), data(i,25)];
    rotMatQTM = [data(i,26), data(i,27), data(i,28);
                 data(i,29), data(i,30), data(i,31);
                 data(i,32), data(i,33), data(i,34)];
    % Calculate Cifference Matrices
    rotMatDiff0 = rotMat0' * rotMatQTM;
    rotMatDiff1 = rotMat1' * rotMatQTM;
    % Convert to Axis-Angle
    rotVecDiff0 = rotm2axang(rotMatDiff0);
    rotVecDiff1 = rotm2axang(rotMatDiff1);
    % Extract angle
    ang0 = rad2deg(rotVecDiff0(4));
    ang1 = rad2deg(rotVecDiff1(4));
    
    % Plot only if QTM matrix is not all zeros
    if all(rotMatQTM(:) ~= 0)
        validIndices(end+1) = i;
    end

    % Choose closest
    if ang0 < ang1
        angChoice(i) = ang0;
    else
        angChoice(i) = ang1;
    end
end

plot(time(validIndices), angChoice(validIndices), '.k', MarkerSize=1)
title("Difference from Axis-Angle: Closest choice")
ylabel("Angle [degrees]")
xlabel("Time [seconds]")
xlim([startTime stopTime])
ylim([0 90])

% Export figure
saveas(angleDiffPlot, ['poseTest_',num2str(testNr), '_angleDiffPlot'])
saveas(angleDiffPlot, ['poseTest_',num2str(testNr), '_angleDiffPlot.png'])

%% XY Projection Plot - Azimuth Angle
% atan2 wrapping 360 deg:
    % https://se.mathworks.com/matlabcentral/
    % answers/202915-converting-atan2-output-to-360-deg
CV0anglesXplot = zeros(length(time), 1);
CV0anglesYplot = zeros(length(time), 1);
CV1anglesXplot = zeros(length(time), 1);
CV1anglesYplot = zeros(length(time), 1);
QTManglesXplot = zeros(length(time), 1);
QTManglesYplot = zeros(length(time), 1);
diffAnglesX = zeros(length(time), 1);
diffAnglesY = zeros(length(time), 1);
CVanglesXchoice = zeros(length(time), 1);
CVanglesYchoice = zeros(length(time), 1);

for i = 1 : length(time)
    x0_x = data(i,8);
    x0_y = data(i,11);
    y0_x = data(i,9);
    y0_y = data(i,12);

    x1_x = data(i,17);
    x1_y = data(i,20);
    y1_x = data(i,18);
    y1_y = data(i,21);

    CV0anglesX = atan2d(x0_y,x0_x) + 360*(x0_y<0);
    CV0anglesY = atan2d(y0_y,y0_x);

    % Experimental
    CV0anglesXplot(i) = CV0anglesX;
    CV0anglesYplot(i) = CV0anglesY;

    CV1anglesX = atan2d(x1_y,x1_x) + 360*(x1_y<0);
    CV1anglesY = atan2d(y1_y,y1_x);

    % Experimental
    CV1anglesXplot(i) = CV1anglesX;
    CV1anglesYplot(i) = CV1anglesY;

    xQTM_x = data(i,26);
    xQTM_y = data(i,29);
    yQTM_x = data(i,27);
    yQTM_y = data(i,30);

    QTManglesX = atan2d(xQTM_y, xQTM_x) + 360*(xQTM_y<0);
    QTManglesY = atan2d(yQTM_y, yQTM_x);

    % Experimental
    QTManglesXplot(i) = QTManglesX;
    QTManglesYplot(i) = QTManglesY;
    
    diff0AnglesX = QTManglesX - CV0anglesX;
    diff0AnglesY = QTManglesY - CV0anglesY;

    diff1AnglesX = QTManglesX - CV1anglesX;
    diff1AnglesY = QTManglesY - CV1anglesY;

    if abs(diff0AnglesX) < abs(diff1AnglesX)
        diffAnglesX(i) = diff0AnglesX;
        CVanglesXchoice(i) = CV0anglesX;
    else
        diffAnglesX(i) = diff1AnglesX;
        CVanglesXchoice(i) = CV1anglesX;
    end
    if abs(diff0AnglesY) < abs(diff1AnglesY)
        diffAnglesY(i) = diff0AnglesY;
        CVanglesYchoice(i) = CV0anglesY;
    else
        diffAnglesY(i) = diff1AnglesY;
        CVanglesYchoice(i) = CV1anglesY;
    end
end

xStart = 90;
yStart = -10;

%%%%%%%% Angle Plot: Vec0 and QTM %%%%%%%%
% Plot only when QTM rotation matrix != 0
%plotY = rad2deg(eul0(:,1));
%validIndices = (QTM ~= 0);
XYprojVec0 = figure(Name="ProjPlotXY_vec0");
sgtitle("Angle projected in XY-plane: Vec0 and QTM")

%%% Plotting X axis angles %%%
validIndices = (data(:,26) ~= 0) & (data(:,29) ~= 0);
subplot(2,1,1)
plot(time(validIndices), CV0anglesXplot(validIndices), '.r')
hold on
plot(time(validIndices), QTManglesXplot(validIndices), '.k', MarkerSize=1)
ylabel("Angle [degrees]")
xlabel("Time [seconds]")
% xlim([0 time(end)])
xlim([startTime stopTime])
ylim([xStart (xStart+360)])

[h, icons] = legend('CV0 $\theta$ from X-axis', 'QTM $\theta$ from Y-axis', ...
    'Interpreter', 'latex', 'Location','best');
% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

%%% Plotting Y axis angles %%%
validIndices = (data(:,27) ~= 0) & (data(:,30) ~= 0);
subplot(2,1,2)
% plot(time(validIndices), diffAnglesY(validIndices), '.g', MarkerSize=1)
plot(time(validIndices), CV0anglesYplot(validIndices), '.g')
hold on
plot(time(validIndices), QTManglesYplot(validIndices), '.k', MarkerSize=1)
ylabel("Angle [degrees]")
xlabel("Time [seconds]")
% xlim([0 time(end)])
xlim([startTime stopTime])
ylim([yStart (yStart+360)])

[h, icons] = legend('CV0 $\theta$ from Y-axis', 'QTM $\theta$ from Y-axis', ...
    'Interpreter', 'latex', 'Location','best');
% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

% Export figure
%set(rot1matDiffPlot,'units','normalized','outerposition',[0 0 1 1])
saveas(XYprojVec0, ['poseTest_',num2str(testNr), '_XYprojVec0'])
% saveas(XYprojVec0, ['poseTest_',num2str(testNr), '_XYprojVec0.png'])
exportgraphics(XYprojVec0, ['poseTest_',num2str(testNr), '_XYprojVec0.pdf'], ...
               'ContentType', 'vector');

%%%%%%%% Angle Plot: Vec1 and QTM %%%%%%%%
% Plot only when QTM rotation matrix != 0
%plotY = rad2deg(eul0(:,1));
%validIndices = (QTM ~= 0);
XYprojVec1 = figure(Name="ProjPlotXY_vec1");
sgtitle("Angle projected in XY-plane: Vec1 and QTM")

%%% Plotting X axis angles %%%
validIndices = (data(:,26) ~= 0) & (data(:,29) ~= 0);
subplot(2,1,1)
plot(time(validIndices), CV1anglesXplot(validIndices), '.r')
hold on
plot(time(validIndices), QTManglesXplot(validIndices), '.k', MarkerSize=1)
ylabel("Angle [degrees]")
xlabel("Time [seconds]")
ylim([xStart (xStart+360)])
% xlim([0 time(end)])
xlim([startTime stopTime])

[h, icons] = legend('CV1 $\theta$ from X-axis', 'QTM $\theta$ from Y-axis', ...
    'Interpreter', 'latex', 'Location','best');
% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

%%% Plotting Y axis angles %%%
validIndices = (data(:,27) ~= 0) & (data(:,30) ~= 0);
subplot(2,1,2)
% plot(time(validIndices), diffAnglesY(validIndices), '.g', MarkerSize=1)
plot(time(validIndices), CV1anglesYplot(validIndices), '.g')
hold on
plot(time(validIndices), QTManglesYplot(validIndices), '.k', MarkerSize=1)
ylabel("Angle [degrees]")
xlabel("Time [seconds]")
ylim([yStart (yStart+360)])
% xlim([0 time(end)])
xlim([startTime stopTime])

[h, icons] = legend('CV1 $\theta$ from Y-axis', 'QTM $\theta$ from Y-axis', ...
    'Interpreter', 'latex', 'Location','best');
% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

% Export figure
%set(rot1matDiffPlot,'units','normalized','outerposition',[0 0 1 1])
saveas(XYprojVec1, ['poseTest_',num2str(testNr), '_XYprojVec1'])
% saveas(XYprojVec1, ['poseTest_',num2str(testNr), '_XYprojVec1.png'])
exportgraphics(XYprojVec1, ['poseTest_',num2str(testNr), '_XYprojVec1.pdf'], ...
               'ContentType', 'vector');

%%%%%%%% Angle Plot: Vector Choice and QTM %%%%%%%%
% Plot only when QTM rotation matrix != 0
%plotY = rad2deg(eul0(:,1));
%validIndices = (QTM ~= 0);
XYprojVecChoice = figure(Name="ProjPlotXY_vecChoice");
sgtitle("Angle projected in XY-plane: Closest rotVector and QTM")

%%% Plotting X axis angles %%%
validIndices = (data(:,26) ~= 0) & (data(:,29) ~= 0);
subplot(2,1,1)
plot(time(validIndices), CVanglesXchoice(validIndices), '.r')
hold on
plot(time(validIndices), QTManglesXplot(validIndices), '.k', MarkerSize=1)
ylabel("Angle [degrees]")
xlabel("Time [seconds]")
ylim([xStart (xStart+360)])
% xlim([0 time(end)])
xlim([startTime stopTime])

[h, icons] = legend('CV $\theta$ from X-axis', 'QTM $\theta$ from Y-axis', ...
    'Interpreter', 'latex', 'Location','best');
% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

%%% Plotting Y axis angles %%%
validIndices = (data(:,27) ~= 0) & (data(:,30) ~= 0);
subplot(2,1,2)
% plot(time(validIndices), diffAnglesY(validIndices), '.g', MarkerSize=1)
plot(time(validIndices), CVanglesYchoice(validIndices), '.g')
hold on
plot(time(validIndices), QTManglesYplot(validIndices), '.k', MarkerSize=1)
ylabel("Angle [degrees]")
xlabel("Time [seconds]")
ylim([yStart (yStart+360)])
% xlim([0 time(end)])
xlim([startTime stopTime])

[h, icons] = legend('CV $\theta$ from Y-axis', 'QTM $\theta$ from Y-axis', ...
    'Interpreter', 'latex', 'Location','best');
% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

% Export figure
%set(rot1matDiffPlot,'units','normalized','outerposition',[0 0 1 1])
saveas(XYprojVecChoice, ['poseTest_',num2str(testNr), '_XYprojVecChoice'])
% saveas(XYprojVecChoice, ['poseTest_',num2str(testNr), '_XYprojVecChoice.png'])
exportgraphics(XYprojVecChoice, ['poseTest_',num2str(testNr), '_XYprojVecChoice.pdf'], ...
               'ContentType', 'vector');

%%%%%%%% Difference Plot %%%%%%%%
% Plot only when QTM rotation matrix != 0
%plotY = rad2deg(eul0(:,1));
validIndices = (QTM ~= 0);
XYprojDiff = figure(Name="ProjPlotXY");
plot(time(validIndices), diffAnglesX(validIndices), '.r', MarkerSize=1)
hold on
plot(time(validIndices), diffAnglesY(validIndices), '.g', MarkerSize=1)
title("Difference in angle projected in XY-plane: Choice")
ylabel("Angle [degrees]")
xlabel("Time [seconds]")
[h, icons] = legend('X-axis diff', 'Y-axis diff');
ylim([-30 30])
xlim([0 time(end)])

% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

% Export figure
%set(rot1matDiffPlot,'units','normalized','outerposition',[0 0 1 1])
saveas(XYprojDiff, ['poseTest_',num2str(testNr), '_XYprojDiff'])
% saveas(XYprojDiff, ['poseTest_',num2str(testNr), '_XYprojDiff.png'])
exportgraphics(XYprojDiff, ['poseTest_',num2str(testNr), '_XYprojDiff.pdf'], ...
               'ContentType', 'vector');
%%%%%%%%%%%%%%%% XY Projection Plot - Azimuth %%%%%%%%%%%%%%%%