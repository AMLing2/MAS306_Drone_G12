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

Q = .1*eye(2);
R = Q;

% one = [1; 2];
% two = eye(2);
% three = one*two*one'

dt = time(2)-time(1);
B = [dt; 1];
B = [B Vd 0*B];
P = B*Q*B';
x = zeros(2);

%%%%%%%%%%%%%%%%%%%% Translation Plotting %%%%%%%%%%%%%%%%%%%%
% startTime = 140;
% stopTime = 186;
startTime = 0;
stopTime = time(end);

transPlot = figure(Name="Plot of Translation comparison");

transIndicesZ = (zQTM ~= trans(3));
plot(time(transIndicesZ),zCV0(transIndicesZ), ...
    '.', 'Color',[109/255, 209/255, 255/255])
hold on
plot(time(transIndicesZ),zQTM(transIndicesZ), '.k', MarkerSize=1)
ylabel('z [m]')
xlabel('Time [seconds]')
xlim([startTime stopTime])

[a, icons] = legend('zCV0', 'zQTM', 'Location','eastoutside');
% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)
%%%%%%%%%%%%%%%%%%%% Translation Plotting %%%%%%%%%%%%%%%%%%%%

for i = 1 : length(time)-1

    dt = time(i+1)-time(i);
    u = zQTM(i)/(dt^2);
    y = [xCV0(i); u];
    
    % Control Input Matrix - Time variant
    B = [dt;
         1];
    
    % Measurement Update - Update
    Kk = (P*C')/(C*P*C' + R);
    x = x + Kk*(y - C*x);
    P = (eye(2)-Kk*C)*P;

    % Time Update - Prediction
    x = A*x + B*u;
    P = A*P*A' + B*Q*B';

    if (zQTM ~= trans(3))
        plot(time(i), x(1))
    end
end