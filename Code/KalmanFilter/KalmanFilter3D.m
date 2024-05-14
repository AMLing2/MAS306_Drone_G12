clc; clear; close all;

%% Implement data
testNr = 4;
fileName = ['ExportedResults_', num2str(testNr), '.csv'];
data = csvread(fileName, 1,0);

time = data(:,1);
xQTM = data(:,2);
yQTM = data(:,3);
zQTM = data(:,4);
xCV0 = data(:,5);
yCV0 = -data(:,6); % Flip axis, different frames
zCV0 = data(:,7);

trans = [0.190 0.143 1.564]; % Physically measured in m

% Markersizes
mSize = 4;
qSize = 3;
kSize = 3.5;
iSize = 6;
rSize = 4;

% xlims: position and speed
startTime = 0;
stopTime = time(end);

%% Interpolation

% New timeseries with constant period
Hz = 100;               % [samples/second]
dt = 1/Hz;              % [seconds]
t = 0 : dt : time(end); % [seconds]

% Change no detection to NaN
xNanIdx = find(xQTM == trans(1));
yNanIdx = find(yQTM == trans(2));
zNanIdx = find(zQTM == trans(3));
xQTMi = xQTM;
xQTMi(xNanIdx) = NaN;
yQTMi = yQTM;
yQTMi(yNanIdx) = NaN;
zQTMi = zQTM;
zQTMi(zNanIdx) = NaN;

% Interpolation: (oldTime, interpBetween, newTime)
xQTMi = interp1(time(~isnan(xQTMi)), xQTMi(~isnan(xQTMi)), t);
yQTMi = interp1(time(~isnan(yQTMi)), yQTMi(~isnan(yQTMi)), t);
zQTMi = interp1(time(~isnan(zQTMi)), zQTMi(~isnan(zQTMi)), t);

% Remove last NaN's - same indices for all
while isnan(zQTMi(end))
    xQTMi(end) = [];
    yQTMi(end) = [];
    zQTMi(end) = [];
    t(end) = [];
end

% Plotting
    % X
xInterp = figure(Name="xQTMinterpolation");
plot(t, xQTMi,'.', Color=[.85 .85 .85], MarkerSize=iSize)
hold on
validIndicesQTMx = (xQTM ~= trans(1));
plot(time(validIndicesQTMx),xQTM(validIndicesQTMx), ...
        '.', 'Color','#9E0000',MarkerSize=rSize)
title('Interpolation of QTM data: x')
[~, icons] = legend('xInterpolatedQTM', 'xQTM', 'Location', 'northeast');
xlim([startTime stopTime])
ylabel('Meters')
xlabel('Seconds')

% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

% Save Figure
set(xInterp,'units','normalized','outerposition',[0 0 1 1])
saveas(xInterp, ['poseTest_',num2str(testNr), '_xInterpolation3D'])
saveas(xInterp, ['poseTest_',num2str(testNr), '_xInterpolation3D.png'])

    % Y
yInterp = figure(Name="yQTMinterpolation");
plot(t, yQTMi,'.', Color=[.85 .85 .85], MarkerSize=iSize)
hold on
validIndicesQTMy = (yQTM ~= trans(2));
plot(time(validIndicesQTMy),yQTM(validIndicesQTMy), ...
        '.', 'Color','#176D00', MarkerSize=rSize)
title('Interpolation of QTM data: y')
[~, icons] = legend('yInterpolatedQTM', 'yQTM', 'Location', 'northeast');
xlim([startTime stopTime])
ylabel('Meters')
xlabel('Seconds')

% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

% Save Figure
set(yInterp,'units','normalized','outerposition',[0 0 1 1])
saveas(yInterp, ['poseTest_',num2str(testNr), '_yInterpolation3D'])
saveas(yInterp, ['poseTest_',num2str(testNr), '_yInterpolation3D.png'])

    % Z
zInterp = figure(Name="zQTMinterpolation");
plot(t, zQTMi,'.', Color=[.85 .85 .85], MarkerSize=iSize)
hold on
validIndicesQTMz = (zQTM ~= trans(3));
plot(time(validIndicesQTMz),zQTM(validIndicesQTMz), ...
        '.', 'Color','#21007F', MarkerSize=rSize)
title('Interpolation of QTM data: z')
[~, icons] = legend('zInterpolatedQTM', 'zQTM', 'Location', 'southwest');
xlim([startTime stopTime])
ylabel('Meters')
xlabel('Seconds')

% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

% Save Figure
set(zInterp,'units','normalized','outerposition',[0 0 1 1])
saveas(zInterp, ['poseTest_',num2str(testNr), '_zInterpolation3D'])
saveas(zInterp, ['poseTest_',num2str(testNr), '_zInterpolation3D.png'])

%% Speed and Acceleration

% Preallocate Space
xDotQTM =    zeros(length(xQTMi)-1,1);
xDotDotQTM = zeros(length(xQTMi)-1,1);
yDotQTM =    zeros(length(yQTMi)-1,1);
yDotDotQTM = zeros(length(yQTMi)-1,1);
zDotQTM =    zeros(length(zQTMi)-1,1);
zDotDotQTM = zeros(length(zQTMi)-1,1);

% Save Recorded Speeds
for i = 2 : length(t)
    xDotQTM(i) = (xQTMi(i) - xQTMi(i-1))/dt;
    yDotQTM(i) = (yQTMi(i) - yQTMi(i-1))/dt;
    zDotQTM(i) = (zQTMi(i) - zQTMi(i-1))/dt;
end

% Save Recorded Accelerations
for i = 2 : length(t)
    xDotDotQTM(i) = (xDotQTM(i) - xDotQTM(i-1))/(dt); % [m/s^2]
    yDotDotQTM(i) = (yDotQTM(i) - yDotQTM(i-1))/(dt); % [m/s^2]
    zDotDotQTM(i) = (zDotQTM(i) - zDotQTM(i-1))/(dt); % [m/s^2]
end

% Set default seed for consistent simulation noise
rng("default")

% Output Noise Density - Datasheet BMI088
xyOND = 160*10^-6;      % [ug/sqrt(Hz)]
zOND  = 190*10^-6;      % [ug/sqrt(Hz)]
g = 9.80665;    % [m/s^2]
% Convert to scalar
xyScale = xyOND*sqrt(Hz)/g;
zScale =   zOND*sqrt(Hz)/g;
% Zero Mean White Gaussian Noise - One for each to get different noise
xWn = wgn(length(xDotDotQTM),1,xyScale, 'linear');
yWn = wgn(length(yDotDotQTM),1,xyScale, 'linear');
zWn = wgn(length(zDotDotQTM),1,zScale, 'linear');

% Noisy IMU signal
xSimIMU = xDotDotQTM + zWn;
ySimIMU = yDotDotQTM + zWn;
zSimIMU = zDotDotQTM + zWn;

%% 3D Kalman Filter

% Initialization
timeTol = 0.01;     % Sync low sps to high sps

% State Vector 6x1
x = [   xQTM(1);
        xDotQTM(1);
        yQTM(1); 
        yDotQTM(1);
        zQTM(1); 
        zDotQTM(1)  ];

% Preallocate Space
xKalman =    zeros(length(t),1);
xDotKalman = zeros(length(t),1);
yKalman =    zeros(length(t),1);
yDotKalman = zeros(length(t),1);
zKalman =    zeros(length(t),1);
zDotKalman = zeros(length(t),1);

% State Transition Matrix 6x6
A = [ 1   dt    0   0   0   0  ;
      0   1     0   0   0   0  ;
      0   0     1   dt  0   0  ;
      0   0     0   1   0   0  ;
      0   0     0   0   1   dt ;
      0   0     0   0   0   1 ];
% Control Input Matrix: 6x3
B = [ dt^2/2    0     0   ;   % x''
        dt      0     0   ;   % x'
        0     dt^2/2  0   ;   % y''
        0      dt     0   ;   % y'
        0       0  dt^2/2 ;   % z''
        0       0    dt   ];  % z'
% Measurement Matrix 3x6
C = [ 1 0 0 0 0 0 ; % x
      0 0 1 0 0 0 ; % y
      0 0 0 0 1 0]; % z

% Identity Matrix
I = eye(6);

% Process Noise w_n 3x3
Q = [ std(xWn)^2        0         0      ;
        0          std(yWn)^2     0      ;
        0               0    std(zWn)^2 ];

% xSigma import: f(x) = p1*x^2 + p2*x + p3
p1x =  0.0330;
p2x = -0.0039;
p3x =  0.0098;
% ySigma import: f(y) = p1*y^2 + p2*y + p3 - 9 intervals
p1y =  0.0419;
p2y = -0.0014;
p3y =  0.0110;
% ySigma import: f(y) = p1*y^2 + p2*y + p3 - 16 intervals
% p1y =  0.0363;
% p2y = -0.0026;
% p3y =  0.0112;
% zSigma import: f(z) = p1*z + p2
p1z =  0.0380;
p2z = -0.0029;

% Start Measurement Noise
v = [ (p1x*xCV0(1)^2 + p2x*xCV0(1) + p3x)^2  ;
      (p1y*yCV0(1)^2 + p2y*yCV0(1) + p3y)^2  ;
                      (p1z*zCV0(1) + p2z)^2 ];

% State Covariance (Matrix)
P = B*Q*B';

% Constant Rate: Kalman Filter
for i = 2 : length(t)

    % Find closest value
    for CViter = 1 : length(time)
        if (abs(time(CViter) - t(i)) < timeTol)
            y = [xCV0(CViter); yCV0(CViter); zCV0(CViter)];

            % Update Measurement Noise
            v = [ (p1x*x(1)^2 + p2x*x(1) + p3x)^2  ; % x
                  (p1y*x(3)^2 + p2y*x(3) + p3y)^2  ; % y
                               (p1z*x(5) + p2z)^2 ]; % z
            R = diag(v); % + 1e-6*I;
            % detected = true;
            break
        else
            y = NaN;
            % detected = false;
        end
    end

    % Update Input: Simulated Acceleration
    u = [xSimIMU(i); ySimIMU(i); zSimIMU(i)];

    % Prediction
    x = A*x + B*u;
    P = A*P*A' + B*Q*B';

    if ~isnan(y) % if detected

        % Measurement Update
        Kk = (P*C')/(C*P*C' + R);
        x = x + Kk*(y - C*x);
        P = (I - Kk*C)*P;
    end

    % Extract Filtered Value of position
    xKalman(i)    = x(1);
    xDotKalman(i) = x(2);
    yKalman(i)    = x(3);
    yDotKalman(i) = x(4);
    zKalman(i)    = x(5);
    zDotKalman(i) = x(6);
end

% Calculate differences
xFilter = [];
yFilter = [];
idxXf= [];
idxYf= [];
for i = 1 : length(t)
    if t(i) > stopTime
        break
    end
    if ~isnan(xQTMi(i)) && (t(i) > startTime)
        xFilter(end+1) = abs(xKalman(i) - xQTMi(i));
        idxXf(end+1) = i;
    end
    if ~isnan(yQTMi(i)) && (t(i) > startTime)
        yFilter(end+1) = abs(yKalman(i) - yQTMi(i));
        idxYf(end+1) = i;
    end
end

xMeas = [];
yMeas = [];
idxXMeas = [];
idxYMeas = [];
for i = 1 : length(time)
    if time(i) > stopTime
        break
    end
    if (xQTM(i) ~= trans(1)) && (xCV0(i) ~= 0) && (time(i) > startTime)
        xMeas(end+1) = abs(xCV0(i) - xQTM(i));
        idxXMeas(end+1) = i;
    end
    if (yQTM(i) ~= trans(2)) && (yCV0(i) ~= 0) && (time(i) > startTime)
        yMeas(end+1) = abs(yCV0(i) - yQTM(i));
        idxYMeas(end+1) = i;
    end
end

% Extract and display statistics - [mm]
    % Filter
XavgFilter = mean(xFilter)*1000;
XstdFilter = std(xFilter)*1000;
YavgFilter = mean(yFilter)*1000;
YstdFilter = std(yFilter)*1000;
table(XavgFilter, XstdFilter, YavgFilter, YstdFilter)
    % X Measurements
XavgMeas = mean((xMeas))*1000;
XstdMeas = std((xMeas))*1000;
YavgMeas = mean((yMeas))*1000;
YstdMeas = std((yMeas))*1000;
table(XavgMeas, XstdMeas, YavgMeas, YstdMeas)

%% X Translation Plotting 

xTransPlot = figure(Name="xTranslationPlot");

% Plot Measured Position from D435 Camera
plot(time,xCV0, '.', 'Color','#CEB7B7',...
    MarkerSize=mSize)
hold on
% Plot Reference from QTM
plot(t,xQTMi, '.k', MarkerSize=qSize)
% Plot Kalman Filter Estimate Position
plot(t, xKalman, '.r', MarkerSize=kSize)

[~, icons] = legend('xCV0', 'xQTMinterp', 'xKalman', 'Location','northeast');
% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

ylabel('Meters')
xlabel('Seconds')
xlim([startTime stopTime])
title('3D Kalman Filter: x Estimate')

set(xTransPlot,'units','normalized','outerposition',[0 0 1 1])
% Export figure
saveas(xTransPlot, ['poseTest_',num2str(testNr), '_xTrans3D'])
saveas(xTransPlot, ['poseTest_',num2str(testNr), '_xTrans3D.png'])

%% Y Translation Plotting 
yTransPlot = figure(Name="yTranslationPlot");

% Plot Measured Position from D435 Camera
plot(time,yCV0, '.', 'Color','#C4D383', ...
    MarkerSize=mSize)
hold on
% Plot Reference from QTM
plot(t,yQTMi, '.k', MarkerSize=qSize)
% Plot Kalman Filter Estimate Position
plot(t, yKalman, '.','Color', '#1D9300', MarkerSize=kSize)

[~, icons] = legend('yCV0', 'yQTMinterp', 'yKalman', 'Location','northeast');
% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

ylabel('Meters')
xlabel('Seconds')
xlim([startTime stopTime])
title('3D Kalman Filter: y Estimate')

set(yTransPlot,'units','normalized','outerposition',[0 0 1 1])
% Export figure
saveas(yTransPlot, ['poseTest_',num2str(testNr), '_yTrans3D'])
saveas(yTransPlot, ['poseTest_',num2str(testNr), '_yTrans3D.png'])

%% Z Translation Plotting 
zTransPlot = figure(Name="zTranslationPlot");

% Plot Measured Position from D435 Camera
plot(time,zCV0, '.', 'Color',[109/255, 209/255, 255/255], ...
    MarkerSize=mSize)
hold on
% Plot Reference from QTM
plot(t,zQTMi, '.k', MarkerSize=qSize)
% Plot Kalman Filter Estimate Position
plot(t, zKalman, '.b', MarkerSize=kSize)

[a, icons] = legend('zCV0', 'zQTMinterp', 'zKalman', 'Location','southwest');
% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

ylabel('Meters')
xlabel('Seconds')
xlim([startTime stopTime])
title('3D Kalman Filter: z Estimate')

set(zTransPlot,'units','normalized','outerposition',[0 0 1 1])
% Export figure
saveas(zTransPlot, ['poseTest_',num2str(testNr), '_zTrans3D'])
saveas(zTransPlot, ['poseTest_',num2str(testNr), '_zTrans3D.png'])

%% x Difference Plotting
% Transpose for plotting
xFilter = xFilter';
xMeas = xMeas';

% Plot Differences between filter and no filter
xDiff3D = figure(Name='xDiff3D');
measurements = plot(time(idxXMeas),xMeas,'.', ...
                'Color','#CEB7B7');
hold on
filter = plot(t(idxXf),xFilter,'.r',MarkerSize=1);
title('x Comparison: abs(error) of measurement and filter')

% hold on
diffSize = 1.5;
diffColor = '#7F0000';
measColor = '#FF9191';
yline((XavgMeas+XstdMeas)/1000,'-','Color',measColor,'LineWidth',diffSize)
yline((XavgMeas-XstdMeas)/1000,'-','Color',measColor,'LineWidth',diffSize)
yline((XavgFilter+XstdFilter)/1000,'--', 'Color', diffColor,'LineWidth',diffSize)
yline((XavgFilter-XstdFilter)/1000,'--', 'Color', diffColor,'LineWidth',diffSize)

% Plot NaNs for legend
measLine = plot(nan, nan, '--', 'Color', measColor);
diffLine = plot(nan, nan, '--', 'Color', diffColor);

ylabel('Meters')
xlabel('Seconds')
[~, icons] = legend([measurements filter measLine diffLine], ...
    'xCV0-xQTM', 'xKalman-xQTMi', 'Measurement Confidence Interval', ...
    'Filter Confidence Interval', 'Location','northeast');
% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)
xlim([startTime stopTime])

set(xDiff3D,'units','normalized','outerposition',[0 0 1 1])
% Export figure
saveas(xDiff3D, ['poseTest_',num2str(testNr), '_xDiff3D'])
saveas(xDiff3D, ['poseTest_',num2str(testNr), '_xDiff3D.png'])

%% y Difference Plotting
% Transpose for plotting
yFilter = yFilter';
yMeas = yMeas';

% Plot Differences between filter and no filter
yDiff3D = figure(Name='yDiff3D');
measurements = plot(time(idxYMeas),yMeas,'.', ...
                'Color','#C4D383');
hold on
filter = plot(t(idxYf),yFilter,'.','Color','#1D9300',MarkerSize=1);
title('y Comparison: abs(error) of measurement and filter')

% hold on
diffSize = 1.5;
diffColor = '#007C1B';
measColor = '#99FFA4';
yline((YavgMeas+YstdMeas)/1000,'-','Color',measColor,'LineWidth',diffSize)
yline((YavgMeas-YstdMeas)/1000,'-','Color',measColor,'LineWidth',diffSize)
yline((YavgFilter+YstdFilter)/1000,'--', 'Color', diffColor,'LineWidth',diffSize)
yline((YavgFilter-YstdFilter)/1000,'--', 'Color', diffColor,'LineWidth',diffSize)

% Plot NaNs for legend
measLine = plot(nan, nan, '--', 'Color', measColor);
diffLine = plot(nan, nan, '--', 'Color', diffColor);

ylabel('Meters')
xlabel('Seconds')
[~, icons] = legend([measurements filter measLine diffLine], ...
    'yCV0-yQTM', 'yKalman-yQTMi', 'Measurement Confidence Interval', ...
    'Filter Confidence Interval', 'Location','northeast');
% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)
xlim([startTime stopTime])

set(yDiff3D,'units','normalized','outerposition',[0 0 1 1])
% Export figure
saveas(yDiff3D, ['poseTest_',num2str(testNr), '_yDiff3D'])
saveas(yDiff3D, ['poseTest_',num2str(testNr), '_yDiff3D.png'])