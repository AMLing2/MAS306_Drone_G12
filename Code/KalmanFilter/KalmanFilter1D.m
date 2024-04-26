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

%% Interpolate QTM data for more consistent values

% New timeseries with constant period
Hz = 100;               % [samples/second]
dt = 1/Hz;              % [seconds]
t = 0 : dt : time(end); % [seconds]

% Change no detection to NaN
nanIdx = find(zQTM == trans(3));
zQTMi = zQTM;
zQTMi(nanIdx) = NaN;
% zQTMiSpline = zQTMi;

% Interpolation: (oldTime, interpBetween, newTime)
zQTMi = interp1(time(~isnan(zQTMi)), zQTMi(~isnan(zQTMi)), t);

%%% Plotting Interpolation %%%

% figure(Name="zQTMinterpolation");
% 
% plot(t, zQTMi,'.y')
% hold on
% % plot(t, zQTMiSpline,'.g', MarkerSize=2)
% plot(time,zQTM, '.k', MarkerSize=1)
% 
% title('Interpolation of QTM data')
% legend('InterpolatedQTM', 'QTM', 'Location', 'best')

%% Kalman Filter

% Measurement Matrix
C = [ 1 0 ];
% Feedthrough Matrix
D = 0;

% Identity matrix
I = eye(2);

% Process Noise w_n
Q = 10; % Covariance Matrix

% Measurement Noise v_n
R = 0.1; % Covariance Matrix

%%%%%%%%%%%%%%%%%%%% Translation Plotting %%%%%%%%%%%%%%%%%%%%
% startTime = 2.5;
% stopTime = 2.5;
startTime = 0;
stopTime = time(end);

transPlot = figure(Name="Plot of Translation comparison");

transIndicesZ = (zQTM ~= trans(3));
plot(time,zCV0, ...
    '.', 'Color',[109/255, 209/255, 255/255])
hold on
plot(t,zQTMi, '.k', MarkerSize=1)
% plot(time(transIndicesZ),zQTM(transIndicesZ), '.k'); %, MarkerSize=1)


%%%%%%%%%%%%%%%%%%%% Translation Plotting %%%%%%%%%%%%%%%%%%%%

% Initialization
timeTol = 0.01;     % Sync low sps to high sps
yPrev = zCV0(1);    % Start measurement
x = [ zQTM(1); 0];  % Start [z, zDot]

% u = (zQTM(i) - zQTM(i-1))/(dt^2);          % [m/s^2]
% x = [ zQTMi(i); (zQTMi(i) - zQTMi(i-1))/(dt) ];

% Preallocate Space
zKalman = zeros(length(t),1);

% State Transition Matrix
A = [ 1   dt ;
      0   1 ];
% Control Input Matrix
B = [ dt^2  ;
      dt   ];
% Covariance?
P = B*Q*B';

% Time-Varying Kalman Filter
for i = 2 : length(t)

    % Find closest value
    for CViter = 1 : length(time)
        if (abs(time(CViter) - t(i)) < timeTol)
            y = zCV0(CViter);
            break
        elseif (i ~= 1)
            y = yPrev;
        end
    end

    % Update Input: Simulated Acceleration
    u = (zQTMi(i) - zQTMi(i-1))/(dt^2);          % [m/s^2]

    deltaTime(i) = dt;
    acc(i) = u;
    
    % Measurement Update - Update
    Kk = (P*C')/(C*P*C' + R);
    x = x + Kk*(y - C*x);
    P = (I - Kk*C)*P;

    % Extract Filtered Value of position
    yEstimated = C*x;
    zKalman(i) = yEstimated;

    % Time Update - Prediction
    x = A*x + B*u;
    P = A*P*A' + B*Q*B';

    % Previous measurement if not detected
    yPrev = y;
end

% Plot Kalman Filter Estimate
plot(t, zKalman, '.r', MarkerSize=1)


[a, icons] = legend('zCV0', 'zQTMinterp', 'zKalman', 'Location','eastoutside');
% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

ylabel('z [m]')
xlabel('Time [seconds]')
xlim([startTime stopTime])
% ylim([-40 40])