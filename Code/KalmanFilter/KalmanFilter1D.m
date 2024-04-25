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
Q = 50; % Covariance Matrix

% Measurement Noise v_n
R = 0.05; % Covariance Matrix

% one = [1; 2];
% two = eye(2);
% three = one*two*one'

% dt = time(2)-time(1);
% B = [0 dt];
% % B = [B Vd 0*B];
% P = B*Q*B';

%%%%%%%%%%%%%%%%%%%% Translation Plotting %%%%%%%%%%%%%%%%%%%%
% startTime = 140;
stopTime = 2.5;
startTime = 0;
% stopTime = time(end);

transPlot = figure(Name="Plot of Translation comparison");

transIndicesZ = (zQTM ~= trans(3));
plot(time(transIndicesZ),zCV0(transIndicesZ), ...
    '.', 'Color',[109/255, 209/255, 255/255])
hold on
plot(time(transIndicesZ),zQTM(transIndicesZ), '.k'); %, MarkerSize=1)

%%%%%%%%%%%%%%%%%%%% Translation Plotting %%%%%%%%%%%%%%%%%%%%
% iter = 1;

% Time-Varying Kalman Filter
for i = 2 : length(time)
    if (zQTM(i) ~= trans(3))
       
        curTime = time(i);                             % [seconds]

        % First iteration
        if i == 2
            dt = time(i) - time(i-1);                  % [seconds]
            B = [dt^2/2; dt];
            P = B*Q*B';
            % Acceleration
            u = (zQTM(i) - zQTM(i-1))/(dt^2);          % [m/s^2]
            x = [ zQTM(i); 0 ];
        else
            dt = curTime - prevTime;                   % [seconds]
            u = ( zQTM(i) - zQTM(prevIter) ) / (dt^2); % [m/s^2]
        end

        % Current iteration variables
        % dt = time(i+1)-time(i);
        y = zCV0(i);

        deltaTime(i) = dt;
        acc(i) = u;
        
        % State Transition Matrix
        A = [ 1   dt ;
              0   1 ];
        % Control Input Matrix
        B = [ dt^2  ;
              dt ];
        
        % Measurement Update - Update
        Kk = (P*C')/(C*P*C' + R);
        x = x + Kk*(y - C*x);
        P = (I - Kk*C)*P;
    
        % Extract Filtered Value of position
        yEstimated = C*x;

        % Time Update - Prediction
        x = A*x + B*u;
        P = A*P*A' + B*Q*B';
    
        % Add points to list
        xKFestimate(i) = yEstimated;
        plot(time(i), yEstimated, '.r', MarkerSize=1)

        % iter = iter + 1;
        prevTime = curTime;
        prevIter = i;
    end
end

[a, icons] = legend('zCV0', 'zQTM', 'zKalman', 'Location','eastoutside');
% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

ylabel('z [m]')
xlabel('Time [seconds]')
xlim([startTime stopTime])
% ylim([-40 40])