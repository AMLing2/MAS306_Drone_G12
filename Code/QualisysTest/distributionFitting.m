clc; clear; close all;

%%%%%%%%%%%%%%% Translation Diff Plotting - Pos %%%%%%%%%%%%%%

for testNr = 0 : 4
    if (testNr ~= 3)
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

% Size of dots in transPlot
tSize = 1;

transDiffPlot = figure(Name='TransDiffPlot2');
sgtitle('Difference between transVector[0] and QTM as function of QTM')
xLimY1 = -0.07;
xLimY2 =  0.12;
yLimY1 = -0.06;
yLimY2 =  0.11;
zLimY1 = -0.25;
zLimY2 =  0.28;

% X plotting
transIndicesX = (xQTM ~= trans(1));
subplot(3,1,1)
xDiff = xQTM(transIndicesX) - xCV0(transIndicesX);
plot(xQTM(transIndicesX),xDiff, '.r', MarkerSize=tSize)
ylabel('difference [m]')
xlabel('x [m]')
xlim([-0.5 0.6])
ylim([xLimY1 xLimY2])

% Y plotting - Remember to flip sign, only axis which is aligned
transIndicesY = (yQTM ~= trans(2));
subplot(3,1,2)
yDiff = yQTM(transIndicesY) - yCV0(transIndicesY);
plot(yQTM(transIndicesY),yDiff, '.g', MarkerSize=tSize)
ylabel('difference [m]')
xlabel('y [m]')
ylim([yLimY1 yLimY2])

% Z plotting
transIndicesZ = (zQTM ~= trans(3));
subplot(3,1,3)
zDiff = zQTM(transIndicesZ) - zCV0(transIndicesZ);
plot(zQTM(transIndicesZ),zDiff, '.', 'Color', ...
    [109/255, 209/255, 255/255], MarkerSize=tSize)
ylabel('difference [m]')
xlabel('z [m]')
xlim([0.1 1.6])
ylim([zLimY1 zLimY2])

% Variables for distributions
distDotSize = 1; % Size of plotted points
boxTrans = 0.07; % Color of boxes outside \pm 1*sigma

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

    % Find horizontal bounds
    left  = min(curQTM);
    right = max(curQTM);

    % Plot intervals
    subplot(3,3,i)
    plot(curQTM, curDiff,'.', 'Color',[109/255, 209/255, 255/255], ...
        MarkerSize=distDotSize)
    ylabel('difference [m]')
    xlabel('z [m]')
    xlim([left right])
    ylim([zLimY1 zLimY2])
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
    % plot(y, zDiff, '.k', MarkerSize=1)
    yline(zExpVal(i)+zStdDev(i), 'k')%, LineWidth=1)
    yline(zPD.mu-zPD.sigma, 'k')%, LineWidth=1)
    % Grey area below
    patch([left, right, right, left], ...
          [zLimY1,       zLimY1,    zPD.mu-zPD.sigma, zPD.mu-zPD.sigma], ...
          'k', 'FaceAlpha', boxTrans, 'EdgeColor', 'none')
    % Grey area above
    patch([left, right, right, left], ...
          [zPD.mu+zPD.sigma, zPD.mu+zPD.sigma, zLimY2, zLimY2], ...
          'k', 'FaceAlpha', boxTrans, 'EdgeColor', 'none')
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

    % Find horizontal bounds
    left  = min(curQTM);
    right = max(curQTM);

    % Plot intervals
    subplot(3,3,i)
    plot(curQTM, curDiff,'.r', MarkerSize=distDotSize)
    ylabel('difference [m]')
    xlabel('x [m]')
    xlim([left right])
    ylim([xLimY1 xLimY2])
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
    % plot(y, xDiff, '.k', MarkerSize=1)
    yline(xExpVal(i)+xStdDev(i), 'k')%, LineWidth=1)
    yline(xPD.mu-xPD.sigma, 'k')%, LineWidth=1)
    % Grey area below
    patch([left, right, right, left], ...
          [xLimY1,       xLimY1,    xPD.mu-xPD.sigma, xPD.mu-xPD.sigma], ...
          'k', 'FaceAlpha', boxTrans, 'EdgeColor', 'none')
    % Grey area above
    patch([left, right, right, left], ...
          [xPD.mu+xPD.sigma, xPD.mu+xPD.sigma, xLimY2, xLimY2], ...
          'k', 'FaceAlpha', boxTrans, 'EdgeColor', 'none')
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
distNums = 9;
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
    
    % Find horizontal bounds
    left  = min(curQTM);
    right = max(curQTM);

    % Plot intervals
    subplot(3,3,i)
    plot(curQTM, curDiff,'.g', MarkerSize=distDotSize)
    ylabel('difference [m]')
    xlabel('y [m]')
    xlim([left, right])
    ylim([yLimY1 yLimY2])
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
    % plot(y, yDiff, '.k', MarkerSize=1)
    yline(yExpVal(i)+yStdDev(i), 'k')%, LineWidth=0.8)
    yline(yPD.mu-yPD.sigma, 'k')%, LineWidth=0.8)
    % Grey area below
    patch([left, right, right, left], ...
          [yLimY1,       yLimY1,    yPD.mu-yPD.sigma, yPD.mu-yPD.sigma], ...
          'k', 'FaceAlpha', boxTrans, 'EdgeColor', 'none')
    % Grey area above
    patch([left, right, right, left], ...
          [yPD.mu+yPD.sigma, yPD.mu+yPD.sigma, yLimY2, yLimY2], ...
          'k', 'FaceAlpha', boxTrans, 'EdgeColor', 'none')
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