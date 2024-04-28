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

% Allocate space for distribution lists
stdDev = zeros(distNums, 1);
expVal = zeros(distNums, 1);
intervalMid = zeros(distNums, 1);

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
    midVal = mean(xlim);
    intervalMid(i) = midVal;

    % Scale for distribution plotting
    xLims = xlim;
    distScale = ( xLims(2) - xLims(1) )/3;

    % Fit Normal Distribution
    pd = fitdist(curDiff, 'normal');
    y = midVal - normalize( pdf(pd, zDiff), 'range' ) * distScale;
    stdDev(i) = pd.sigma;
    expVal(i) = pd.mu;
    
    % Plot Distribution
    hold on 
    plot(y, zDiff, '.k', MarkerSize=1)
end

% Add a bit space to the figure
fig = gcf;
fig.Position(3) = fig.Position(3) + 250;
% Add common legend outside subplots
[lgd, icons] = legend('zDiff', 'Distribution');
lgd.Position(1) = 0.0;
lgd.Position(2) = 0.5;
% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)


