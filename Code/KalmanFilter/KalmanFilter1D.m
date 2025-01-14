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

% xlims: position and speed
startTime = 0;
stopTime = time(end);

%% Interpolation

% New timeseries with constant period
Hz = 100;               % [samples/second]
dt = 1/Hz;              % [seconds]
t = 0 : dt : time(end); % [seconds]

% Change no detection to NaN
nanIdx = find(zQTM == trans(3));
zQTMi = zQTM;
zQTMi(nanIdx) = NaN;

% Interpolation: (oldTime, interpBetween, newTime)
zQTMi = interp1(time(~isnan(zQTMi)), zQTMi(~isnan(zQTMi)), t);

% Remove NaN's after data
while isnan(zQTMi(end))
    zQTMi(end) = [];
    t(end) = [];
end

% Interpolation Plotting
interpPlot = figure(Name="zQTMinterpolation");
plot(t, zQTMi,'.', Color=[.85 .85 .85])
hold on
validIndicesQTM = (zQTM ~= trans(3));
plot(time(validIndicesQTM),zQTM(validIndicesQTM), '.',...
        'Color', '#21007F', MarkerSize=1)
title('Interpolation of QTM data')
[~, icons] = legend('InterpolatedQTM', 'QTM', 'Location', 'southwest');
xlim([startTime stopTime])
ylabel('Meters')
xlabel('Seconds')

% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

% Export figure
set(interpPlot,'units','normalized','outerposition',[0 0 1 1])
saveas(interpPlot, ['poseTest_',num2str(testNr), '_interpolation'])
saveas(interpPlot, ['poseTest_',num2str(testNr), '_interpolation.png'])

%% Speed and Acceleration

% Preallocate Space
zDotQTM =    zeros(length(zQTMi)-1,1);
zDotDotQTM = zeros(length(zQTMi)-1,1);

% Save Recorded Speed z'
for i = 2 : length(t)
    zDotQTM(i) = (zQTMi(i) - zQTMi(i-1))/dt; % [m/s]
end

% Save Recorded Acceleration z''
for i = 2 : length(t)
    zDotDotQTM(i) = (zDotQTM(i) - zDotQTM(i-1))/(dt); % [m/s^2]
end

% Set default seed for consistent simulation noise
rng("default")

% Output Noise Density - Datasheet BMI088
ond = 190*10^-6;    % [g/sqrt(Hz)]
g = 9.80665;        % [m/s^2]
% Convert to scalar
scale = ond*sqrt(Hz)/g;
% Zero Mean White Gaussian Noise
Wn = wgn(length(zDotDotQTM),1,scale, 'linear');

% Noisy IMU signal
simIMU = zDotDotQTM + Wn;
% Standard Deviation for Covariance (matrix) Q
stdDevIMU = std(Wn);

%% Kalman Filter

% Initialization
timeTol = 0.01;     % Sync low sps to high sps
x = [ zQTM(1); zDotQTM(1)];  % Start [z, zDot]

% Preallocate Space
zKalman = zeros(length(t),1);
zDotKalman = zeros(length(t),1);

% State Transition Matrix
A = [ 1   dt ;
      0   1 ];
% Control Input Matrix
B = [ dt^2/2  ;
      dt     ];
% Measurement Matrix
C = [ 1 0 ];

% Identity Matrix
I = eye(2);

% Process Noise w_n
Q = cov(Wn); % Covariance (Matrix)
Q2 = stdDevIMU^2;
Q3 = var(Wn);

% zSigma import: f(z) = p1*z + p2
p1 =  0.0380;
p2 = -0.0029;

% Measurement Noise v_n
% R = 0.15; % Covariance (Matrix)

% State Covariance (Matrix)
P = B*Q*B';

% Constant Rate: Kalman Filter
for i = 2 : length(t)

    % Find closest value
    for CViter = 1 : length(time)
        if (abs(time(CViter) - t(i)) < timeTol)
            y = zCV0(CViter);
            % detected = true;
            break
        else
            y = NaN;
            % detected = false;
        end
    end

    % Update Input: Simulated Acceleration
    u = simIMU(i);

    % Update Measurement Noise
    R = (p1*y + p2)^2;
    
    % Prediction
    x = A*x + B*u;
    P = A*P*A' + B*Q*B';

    % Measurement Update
    if ~isnan(y) % if detected
        Kk = (P*C')/(C*P*C' + R);
        x = x + Kk*(y - C*x);
        P = (I - Kk*C)*P;
    end

    % Extract Filtered Value of position
    zKalman(i)    = x(1);
    zDotKalman(i) = x(2);
end

% Calculate differences
diff = [];
idxDiff = [];
for i = 1 : length(t)
    if t(i) > stopTime
        break
    end
    if ~isnan(zQTMi(i)) && (t(i) > startTime)
        diff(end+1) = abs(zKalman(i) - zQTMi(i));
        idxDiff(end+1) = i;
    end
end

meas = [];
idxMeas = [];
for i = 1 : length(time)
    if time(i) > stopTime
        break
    end
    if (zQTM(i) ~= trans(3)) && (time(i) > startTime)
        meas(end+1) = abs(zCV0(i) - zQTM(i));
        idxMeas(end+1) = i;
    end
end

% Extract and display statistics - [mm]
    % Difference
avgDiff = mean((diff))*1000;
stdDiff = std((diff))*1000;
table(avgDiff, stdDiff)
    % Measurements
avgMeas = mean((meas))*1000;
stdMeas = std((meas))*1000;
table(avgMeas, stdMeas)

%% Translation Plotting
transPlot = figure(Name="TranslationPlot");

% Plot Measured Position from D435 Camera
plot(time,zCV0, ...
    '.', 'Color',[109/255, 209/255, 255/255])
hold on
% Plot Reference from QTM
plot(t,zQTMi, '.k', MarkerSize=1)
% Plot Kalman Filter Estimate Position
plot(t, zKalman, '.b', MarkerSize=1)

[~, icons] = legend('zCV0', 'zQTMinterp', 'zKalman', 'Location', 'southwest');
% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

ylabel('Meters')
xlabel('Seconds')
xlim([startTime stopTime])
title('1D Kalman Filter')

set(transPlot,'units','normalized','outerposition',[0 0 1 1])
% Export figure
saveas(transPlot, ['poseTest_',num2str(testNr), '_transKalmanPlot'])
saveas(transPlot, ['poseTest_',num2str(testNr), '_transKalmanPlot.png'])

%% Difference Plotting
% Transpose for plotting
diff = diff';
meas = meas';

% Plot Differences between filter and no filter
filterNoFilter1D = figure(Name='FilterNoFilter1D');
measurements = plot(time(idxMeas),meas,'.', ...
                'Color',[109/255, 209/255, 255/255]);
hold on
filter = plot(t(idxDiff),diff,'.b', MarkerSize=1);
title('Comparison: abs(error) of measurement and filter')

% hold on
diffSize = 1.5;
diffColor = '#0000B5';
measColor = '#7EBBD6';
% yline((avgMeas)/1000,'-','Color',measColor,'LineWidth',diffSize)
yline((avgMeas+stdMeas)/1000,'--','Color',measColor,'LineWidth',diffSize)
yline((avgMeas-stdMeas)/1000,'--','Color',measColor,'LineWidth',diffSize)
% yline((avgDiff)/1000,'-', 'Color', diffColor, 'LineWidth',diffSize)
yline((avgDiff+stdDiff)/1000,'--', 'Color', diffColor,'LineWidth',diffSize)
yline((avgDiff-stdDiff)/1000,'--', 'Color', diffColor,'LineWidth',diffSize)

% Plot NaNs for legend
measLine = plot(nan, nan, '--', 'Color', measColor);
diffLine = plot(nan, nan, '--', 'Color', diffColor);

ylabel('Meters')
xlabel('Seconds')
[~, icons] = legend([measurements filter measLine diffLine], ...
    'zCV0-zQTM', 'Kalman-zQTMi', 'Measurement Confidence Interval', ...
    'Filter Confidence Interval', 'Location','northeast');
% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)
xlim([startTime stopTime])

set(filterNoFilter1D,'units','normalized','outerposition',[0 0 1 1])
% Export figure
saveas(filterNoFilter1D, ['poseTest_',num2str(testNr), '_filterNoFilter1D'])
saveas(filterNoFilter1D, ['poseTest_',num2str(testNr), '_filterNoFilter1D.png'])

%% Speed Plotting
speedPlot = figure(Name="SpeedPlot");

% Plot Reference Speed
plot(t,zDotQTM, '.k')
hold on

% Plot Estimated Speed
plot(t,zDotKalman, '.', 'Color', '#B1A9D8', MarkerSize=1)

[~, icons] = legend('zDotQTM', 'zDotKalman', 'Location','northeast');
% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

ylabel('zDot [m/s]')
xlabel('Time [seconds]')
xlim([startTime stopTime])
title("Speed: Kalman' and QTM'")

set(speedPlot,'units','normalized','outerposition',[0 0 1 1])

%% Acceleration Plotting
accPlot = figure(Name="AccPlot");
% Plot QTM Acceleration
plot(t,zDotDotQTM, '.k')
hold on
% Plot Simulation with Noise
plot(t,simIMU, '.', 'Color', '#F7B9EA', MarkerSize=1)

[a, icons] = legend('IMUsim', 'zDotDotQTM', 'Location','northeast');
% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

ylabel('zDotDot [m/s^2]')
xlabel('Time [seconds]')
xlim([startTime stopTime])
title("Acceleration: simIMU and QTM''")

set(accPlot,'units','normalized','outerposition',[0 0 1 1])