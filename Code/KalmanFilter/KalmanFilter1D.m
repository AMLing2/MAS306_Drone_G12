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

% Measurement Matrix
C = [ 1 0 ];
% Feedthrough Matrix
D = 0;

% Identity matrix
I = eye(2);

% Process Noise w_n
Q = 0.1; % Covariance Matrix

% Measurement Noise v_n
R = 0.05; % Covariance Matrix

% one = [1; 2];
% two = eye(2);
% three = one*two*one'

dt = time(2)-time(1);
B = [0 dt];
% B = [B Vd 0*B];
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

hold on
% Time-Varying Kalman Filter
for i = 1 : length(time)-1

    % Current iteration variables
    dt = time(i+1)-time(i);
    u = zQTM(i)/(dt^2);
    y = xCV0(i);
    
    % State Transition Matrix
    A = [ 1   dt ;
          0   1 ];
    % Control Input Matrix - Time variant
    B = [ 0  ;
          dt ];
    
    % Measurement Update - Update
    Kk = (P*C')/(C*P*C' + R);
    x = x + Kk*(y - C*x);
    P = (I - Kk*C)*P;

    % Time Update - Prediction
    x = A*x + B*u;
    P = A*P*A' + B*Q*B';

    % Add points to list
    if (zQTM(i) ~= trans(3))
        xKFestimate(i) = x(1);
        plot(time(i), x(1), '.r')
    end
end