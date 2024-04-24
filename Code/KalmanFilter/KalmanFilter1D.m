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

%% Kalman Filter

% State Transition Matrix
A = [ 0   1  ;
      0   0 ];

% Measurement Matrix
C = [ 1 0;
      0 0 ];
% Feedthrough Matrix
D = [ 0;
      1];

% Disturbance Covariance
Vd = .1*eye(2);

% Noice Covariance
Vn = .1*eye(2);

u = zeros(2);
y = zeros(2);

for i = 1 : length(time)-1
    dt = time(i+1)-time(i);
    u = [zQTM(i)/(dt^2) zQTM(i)/(dt^2)];
    y = [xCV0(i); u(1)];
    
    % Control Input Matrix - Time variant
    B = [dt;
         1];

    % Build State Space Model
    sysSS = ss(A,B,C,D);
    
    % Build Kalman Filter
    [Kf, P, E] = lqe(A, Vd, C, Vd, Vn);
    
    % New State Space for Kalman Filter
    sysKF = ss(A-Kf*C, [B Kf], eye(2), 0*[B Kf]);
    
    % Extract Kalman Filter Values
    % [zKF, t] = lsim(sysKF, [u; y], time(i));
    [zKF, L,~,Mx,Z] = kalman(sysKF,Vd,Vn);
    zKF = zKF(1,:);

    if (zQTM ~= trans(3))
        plot(time(i), zCV0(i))
        hold on
        plot(time(i), zKF)
    end
end

%% Plotting

% startTime = 140;
% stopTime = 186;
% % startTime = 0;
% % stopTime = time(end);
% 
% %%%%%%%%%%%%%%%%%%%% Translation Plotting %%%%%%%%%%%%%%%%%%%%
% transPlot = figure(Name="Plot of Translation comparison");
% 
% transIndicesZ = (zQTM ~= trans(3));
% plot(time(transIndicesZ),zCV0(transIndicesZ), ...
%     '.', 'Color',[109/255, 209/255, 255/255])
% hold on
% plot(time(transIndicesZ),zQTM(transIndicesZ), '.k', MarkerSize=1)
% ylabel('z [m]')
% xlabel('Time [seconds]')
% xlim([startTime stopTime])
% 
% [a, icons] = legend('zCV0', 'zQTM', 'Location','eastoutside');
% % Change size of legend icons
% icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
% set(icons, 'MarkerSize', 20)