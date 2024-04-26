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

%%%%%%%%%%%%%%%%%%%% Translation Plotting %%%%%%%%%%%%%%%%%%%%
% startTime = 2.5;
% stopTime = 2.5;
startTime = 0;
stopTime = time(end);

transPlot = figure(Name="TranslationPlot");

transIndicesZ = (zQTM ~= trans(3));
plot(time,zCV0, ...
    '.', 'Color',[109/255, 209/255, 255/255])
hold on
plot(t,zQTMi, '.k', MarkerSize=1)
% plot(time(transIndicesZ),zQTM(transIndicesZ), '.k'); %, MarkerSize=1)

%%%%%%%%%%%%%%%%%%%% Translation Plotting %%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%% Speed Plotting %%%%%%%%%%%%%%%%%%%%
speedPlot = figure(Name="SpeedPlot");

% Preallocate Space
zDotQTM = zeros(length(zCV0)-1,1);
% Save Recorded Speed
for i = 2 : length(zQTMi)
    zDotQTM(i) = (zQTMi(i) - zQTMi(i-1))/dt;
end

% zDotQTM = normalize(zDotQTM); % Normalization for troubleshooting
plot(t,zDotQTM, '.k')

%%%%%%%%%%%%%%%%%%%% Speed Plotting %%%%%%%%%%%%%%%%%%%%
figure(transPlot)
% Initialization
timeTol = 0.01;     % Sync low sps to high sps
yPrev = zCV0(1);    % Start measurement
x = [ zQTM(1); zDotQTM(1)];  % Start [z, zDot]

% u = (zQTM(i) - zQTM(i-1))/(dt^2);          % [m/s^2]
% x = [ zQTMi(i); (zQTMi(i) - zQTMi(i-1))/(dt) ];

% Preallocate Space
zKalman = zeros(length(t),1);
IMUsim  = zeros(length(t),1);
zDotKalman = zeros(length(t),1);
% detected = false;
% detectList = zeros(length(t), 1);

% State Transition Matrix
A = [ 1   dt ;
      0   1 ];
% Control Input Matrix
B = [ dt^2/2  ;
      dt     ];
% Measurement Matrix
C = [ 1 0 ];
% Feedthrough Matrix
D = 0;

% Identity matrix
I = eye(2);

% Process Noise w_n
Q = 500; % Covariance Matrix
% Measurement Noise v_n
R = 0.1; % Covariance Matrix

% Covariance?
P = B*Q*B';

% Constant Rate: Kalman Filter
for i = 2 : length(t)

    % Find closest value
    for CViter = 1 : length(time)
        if (abs(time(CViter) - t(i)) < timeTol)
            y = zCV0(CViter);
            % detected = true;
            break
        elseif (i ~= 1)
            y = NaN;
            % y = yPrev;
            % detected = false;
        end
    end
    % detectList(i) = detected;

    % Update Input: Simulated Acceleration
    u = (zQTMi(i) - zQTMi(i-1))/(dt^2);          % [m/s^2]
    IMUsim(i) = u;
    
    % Measurement Update - Update
    % if detected
    if ~isnan(y)
        Kk = (P*C')/(C*P*C' + R);
        x = x + Kk*(y - C*x);
        P = (I - Kk*C)*P;
    end

    % Extract Filtered Value of position
    zKalman(i)    = x(1);
    zDotKalman(i) = x(2);

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


%%%%%%%%%%%%%%%%%% Plot Speeds %%%%%%%%%%%%%%%%%%
figure(speedPlot)
hold on

% zDotKalman = normalize(zDotKalman); % Normalization for troubleshooting
plot(t,zDotKalman, '.r', MarkerSize=1)

[a, icons] = legend('zDotQTM', 'zDotKalman', 'Location','eastoutside');
% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

ylabel('zDot [m/s]')
xlabel('Time [seconds]')
xlim([startTime stopTime])