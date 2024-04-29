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
xLimsY = 0.15;
yLimsY = 0.15;
zLimsY = 0.3;

% X plotting
transIndicesX = (xQTM ~= trans(1));
subplot(3,1,1)
xDiff = xQTM(transIndicesX) - xCV0(transIndicesX);
plot(xQTM(transIndicesX),xDiff, '.r')
ylabel('difference [m]')
xlabel('x [m]')
% xlim([startTime stopTime])
ylim([-xLimsY xLimsY])

% Y plotting - Remember to flip sign, only axis which is aligned
transIndicesY = (yQTM ~= trans(2));
subplot(3,1,2)
yDiff = yQTM(transIndicesY) - yCV0(transIndicesY);
plot(yQTM(transIndicesY),yDiff, '.g')
ylabel('difference [m]')
xlabel('y [m]')
% xlim([startTime stopTime])
ylim([-yLimsY yLimsY])

% Z plotting
transIndicesZ = (zQTM ~= trans(3));
subplot(3,1,3)
zDiff = zQTM(transIndicesZ) - zCV0(transIndicesZ);
plot(zQTM(transIndicesZ),zDiff, '.', 'Color',[109/255, 209/255, 255/255])
ylabel('difference [m]')
xlabel('z [m]')
% xlim([startTime stopTime])
ylim([-zLimsY zLimsY])

%% Z Distributions

zQTM = zQTM(transIndicesZ);

% Sort the lists to ascending order
[zQTMsorted, zQTMindices] = sort(zQTM);
zDiffSorted = zDiff(zQTMindices);

% Split the data into evenly distributed intervals
distNums = 9;
intervals = linspace(zQTMsorted(1), zQTMsorted(end), (distNums+1));

% Find indices closest to bounds of intervals
[~, closestIdx] = min( abs(zQTMsorted - intervals));

% Allocate space for distribution lists
zStdDev = zeros(distNums, 1);
zExpVal = zeros(distNums, 1);
zIntervalMid = zeros(distNums, 1);

zIntervalPlots = figure(Name="zIntervalPlots");
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
    ylim([-zLimsY zLimsY])
    midVal = mean(xlim);
    zIntervalMid(i) = midVal;

    % Scale for distribution plotting
    xLims = xlim;
    distScale = ( xLims(2) - xLims(1) )/3;

    % Fit Normal Distribution
    zPD = fitdist(curDiff, 'normal');
    y = midVal - normalize( pdf(zPD, zDiff), 'range' ) * distScale;
    zStdDev(i) = zPD.sigma;
    zExpVal(i) = zPD.mu;
    
    % Plot Distribution
    hold on 
    plot(y, zDiff, '.k', MarkerSize=1)
end

% Add a bit space to the figure
fig = gcf;
fig.Position(3) = fig.Position(3) + 250;
% Add common legend outside subplots
[lgd, icons] = legend('zDiff', 'zDistribution');
lgd.Position(1) = 0.0;
lgd.Position(2) = 0.5;
% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

%% X Distributions

xQTM = xQTM(transIndicesX);

% Sort the lists to ascending order
[xQTMsorted, xQTMindices] = sort(xQTM);
xDiffSorted = xDiff(xQTMindices);

% Split the data into evenly distributed intervals
distNums = 9;
intervals = linspace(xQTMsorted(1), xQTMsorted(end), (distNums+1));

% Find indices closest to bounds of intervals
[~, closestIdx] = min( abs(xQTMsorted - intervals));

% Allocate space for distribution lists
xStdDev = zeros(distNums, 1);
xExpVal = zeros(distNums, 1);
xIntervalMid = zeros(distNums, 1);

xIntervalPlots = figure(Name="xIntervalPlots");
sgtitle('Interval Plots of x-axis')

for i = 1 : distNums
    % Save current interval values
    curQTM = xQTMsorted( closestIdx(i):closestIdx(i+1) );
    curDiff = xDiffSorted( closestIdx(i):closestIdx(i+1) );

    % Plot intervals
    subplot(3,3,i)
    plot(curQTM, curDiff,'.r')
    ylabel('difference [m]')
    xlabel('x [m]')
    ylim([-xLimsY xLimsY])
    midVal = mean(xlim);
    xIntervalMid(i) = midVal;

    % Scale for distribution plotting
    xLims = xlim;
    distScale = ( xLims(2) - xLims(1) )/3;

    % Fit Normal Distribution
    xPD = fitdist(curDiff, 'normal');
    y = midVal - normalize( pdf(xPD, xDiff), 'range' ) * distScale;
    xStdDev(i) = xPD.sigma;
    xExpVal(i) = xPD.mu;
    
    % Plot Distribution
    hold on 
    plot(y, xDiff, '.k', MarkerSize=1)
end

% Add a bit space to the figure
fig = gcf;
fig.Position(3) = fig.Position(3) + 250;
% Add common legend outside subplots
[lgd, icons] = legend('xDiff', 'xDistribution');
lgd.Position(1) = 0.0;
lgd.Position(2) = 0.5;
% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

%% Y Distributions

yQTM = yQTM(transIndicesX);

% Sort the lists to ascending order
[yQTMsorted, yQTMindices] = sort(yQTM);
yDiffSorted = yDiff(yQTMindices);

% Split the data into evenly distributed intervals
distNums = 16;
intervals = linspace(yQTMsorted(1), yQTMsorted(end), (distNums+1));

% Find indices closest to bounds of intervals
[~, closestIdx] = min( abs(yQTMsorted - intervals));

% Allocate space for distribution lists
yStdDev = zeros(distNums, 1);
yExpVal = zeros(distNums, 1);
yIntervalMid = zeros(distNums, 1);

yIntervalPlots = figure(Name="yIntervalPlots");
sgtitle('Interval Plots of y-axis')

for i = 1 : distNums
    % Save current interval values
    curQTM = yQTMsorted( closestIdx(i):closestIdx(i+1) );
    curDiff = yDiffSorted( closestIdx(i):closestIdx(i+1) );

    % Plot intervals
    subplot(4,4,i)
    plot(curQTM, curDiff,'.g')
    ylabel('difference [m]')
    xlabel('y [m]')
    ylim([-yLimsY yLimsY])
    midVal = mean(xlim);
    yIntervalMid(i) = midVal;

    % Scale for distribution plotting
    xLims = xlim;
    distScale = ( xLims(2) - xLims(1) )/3;

    % Fit Normal Distribution
    yPD = fitdist(curDiff, 'normal');
    y = midVal - normalize( pdf(yPD, yDiff), 'range' ) * distScale;
    yStdDev(i) = yPD.sigma;
    yExpVal(i) = yPD.mu;
    
    % Plot Distribution
    hold on 
    plot(y, yDiff, '.k', MarkerSize=1)
end

% Add a bit space to the figure
fig = gcf;
fig.Position(3) = fig.Position(3) + 250;
% Add common legend outside subplots
[lgd, icons] = legend('yDiff', 'yDistribution');
lgd.Position(1) = 0.0;
lgd.Position(2) = 0.5;
% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)