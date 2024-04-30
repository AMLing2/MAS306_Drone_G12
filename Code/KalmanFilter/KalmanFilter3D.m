clc; clear; close all;

%% Implement data
testNr = 2;
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

%%%%%%%%%%%%%%%% Interpolation Setup %%%%%%%%%%%%%%%%

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

%%%%% Interpolation Plotting %%%%%
%     % X
% figure(Name="xQTMinterpolation");
% plot(t, xQTMi,'.y')
% hold on
% % plot(t, zQTMiSpline,'.g', MarkerSize=2)
% plot(time,xQTM, '.k', MarkerSize=1)
% title('Interpolation of QTM data: x')
% legend('xInterpolatedQTM', 'xQTM', 'Location', 'best')
% 
%     % Y
% figure(Name="yQTMinterpolation");
% plot(t, yQTMi,'.y')
% hold on
% % plot(t, zQTMiSpline,'.g', MarkerSize=2)
% plot(time,yQTM, '.k', MarkerSize=1)
% title('Interpolation of QTM data: y')
% legend('yInterpolatedQTM', 'yQTM', 'Location', 'best')
% 
%     % Z
% figure(Name="zQTMinterpolation");
% plot(t, zQTMi,'.y')
% hold on
% % plot(t, zQTMiSpline,'.g', MarkerSize=2)
% plot(time,zQTM, '.k', MarkerSize=1)
% title('Interpolation of QTM data: z')
% legend('zInterpolatedQTM', 'zQTM', 'Location', 'best')

%%%%%%%%%%%%%%%% Interpolation Setup %%%%%%%%%%%%%%%%

%%%%%%%%%% Speed/Acceleration Setup %%%%%%%%%%

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

% Output Noise Density - Datasheet BMI088
xyOND = 160;      % [ug/sqrt(Hz)]
zOND = 190;      % [ug/sqrt(Hz)]
g = 9.80665;    % [m/s^2]
% Convert to scalar
xyScale = xyOND*g/(10^6)/sqrt(Hz);
zScale =   zOND*g/(10^6)/sqrt(Hz);
% Zero Mean White Gaussian Noise - One for each to get different noise
xWn = wgn(length(xDotDotQTM),1,xyScale, 'linear');
yWn = wgn(length(yDotDotQTM),1,xyScale, 'linear');
zWn = wgn(length(zDotDotQTM),1,zScale, 'linear');

% Noisy IMU signal
xSimIMU = xDotDotQTM + zWn;
ySimIMU = yDotDotQTM + zWn;
zSimIMU = zDotDotQTM + zWn;
% Standard Deviation for Covariance (matrix) Q
% stdDevIMU = std(Wn);

%%%%%%%%%% Speed/Acceleration Setup %%%%%%%%%%

% Initialization
timeTol = 0.01;     % Sync low sps to high sps
% x = [   zQTM(1); 
%         zDotQTM(1)  ];  % Start [z, zDot]
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

% State Transition Matrix
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
% Measurement Matrix
C = [ 1 0 0 0 0 0 ; % x
      0 0 0 0 0 0 ;
      0 0 1 0 0 0 ; % y
      0 0 0 0 0 0 ;
      0 0 0 0 1 0 ; % z
      0 0 0 0 0 0 ];
% Feedthrough Matrix
D = 0;

% Identity Matrix
I = eye(6);

% Process Noise w_n
% Q = cov(zWn); % Covariance (Matrix)
Q = 0.001*eye(3);

% xSigma import: f(x) = p1*x^2 + p2*x + p3
p1x =  0.0330;
p2x = -0.0039;
p3x =  0.0098;
% xSigma import: f(y) = p1*y^2 + p2*y + p3
p1y =  0.0419;
p2y = -0.0014;
p3y =  0.0110;
% zSigma import: f(z) = p1*z + p2
p1z =  0.0380;
p2z = -0.0029;

% Measurement Noise v_n
R = 0.1; % Covariance (Matrix)

% State Covariance (Matrix)
P = B*Q*B';

%%%%%%%% Constant Rate: Kalman Filter %%%%%%%%
for i = 2 : length(t)

    % Find closest value
    for CViter = 1 : length(time)
        if (abs(time(CViter) - t(i)) < timeTol)
            y = [xCV0(CViter); 0; yCV0(CViter); 0; zCV0(CViter); 0];
            % detected = true;
            break
        else
            y = NaN;
            % detected = false;
        end
    end

    % Update Input: Simulated Acceleration
    u = [xDotDotQTM(i); yDotDotQTM(i); zDotDotQTM(i)];

    % Update Measurement Noise
    % R = p1*y + p2;
    
    % Prediction
    x = A*x + B*u;
    P = A*P*A' + B*Q*B';

    % Measurement Update
    if ~isnan(y) % if detected
        Kk = (P*C')/(C*P*C' + R*I);
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
%%%%%%%% Constant Rate: Kalman Filter %%%%%%%%

% xlims: position and speed
startTime = 0;
stopTime = time(end);

%%%%%%%%%%%%%%%%% x Translation Plotting %%%%%%%%%%%%%%%%%
xTransPlot = figure(Name="xTranslationPlot");

% Plot Measured Position from D435 Camera
plot(time,xCV0, ...
    '.', 'Color',[109/255, 209/255, 255/255])
hold on
% Plot Reference from QTM
plot(t,xQTMi, '.k', MarkerSize=1)
% Plot Kalman Filter Estimate Position
plot(t, xKalman, '.r', MarkerSize=1)

[a, icons] = legend('xCV0', 'xQTMinterp', 'xKalman', 'Location','eastoutside');
% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

ylabel('x [m]')
xlabel('Time [seconds]')
xlim([startTime stopTime])
%%%%%%%%%%%%%%%%% x Translation Plotting %%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%% y Translation Plotting %%%%%%%%%%%%%%%%%
yTransPlot = figure(Name="yTranslationPlot");

% Plot Measured Position from D435 Camera
plot(time,yCV0, ...
    '.', 'Color',[109/255, 209/255, 255/255])
hold on
% Plot Reference from QTM
plot(t,yQTMi, '.k', MarkerSize=1)
% Plot Kalman Filter Estimate Position
plot(t, yKalman, '.r', MarkerSize=1)

[a, icons] = legend('yCV0', 'yQTMinterp', 'yKalman', 'Location','eastoutside');
% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

ylabel('y [m]')
xlabel('Time [seconds]')
xlim([startTime stopTime])
%%%%%%%%%%%%%%%%% Translation Plotting %%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%% z Translation Plotting %%%%%%%%%%%%%%%%%
zTransPlot = figure(Name="zTranslationPlot");

% Plot Measured Position from D435 Camera
plot(time,zCV0, ...
    '.', 'Color',[109/255, 209/255, 255/255])
hold on
% Plot Reference from QTM
plot(t,zQTMi, '.k', MarkerSize=1)
% Plot Kalman Filter Estimate Position
plot(t, zKalman, '.r', MarkerSize=1)

[a, icons] = legend('zCV0', 'zQTMinterp', 'zKalman', 'Location','eastoutside');
% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

ylabel('z [m]')
xlabel('Time [seconds]')
xlim([startTime stopTime])
%%%%%%%%%%%%%%%%% Translation Plotting %%%%%%%%%%%%%%%%%