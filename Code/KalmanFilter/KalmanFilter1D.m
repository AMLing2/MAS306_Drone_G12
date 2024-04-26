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

% for i = 1 : length(zQTM)
%     if zQTM(i) == 0
%         zQTM(i) = z
%     end
% end

% New timeseries with constant period
Hz = 100;               % [samples/second]
dt = 1/Hz;              % [seconds]
t = 0 : dt : time(end); % [seconds]

% Change no detection to NaN
nanIdx = find(zQTM == trans(3));
zQTMi = zQTM;
zQTMi(nanIdx) = NaN;
% zQTMiSpline = zQTMi;

% Find the indices of the NaN values in the position list
nan_idx = find(isnan(zQTMi));

% Interpolate the missing values in the position list using the time values
zQTMi =       interp1(time(~isnan(zQTMi)), zQTMi(~isnan(zQTMi)), t);
% zQTMiSpline = interp1(time(~isnan(zQTMiSpline)), zQTMi(~isnan(zQTMiSpline)), t, 'spline');

% Interpolate NaN values
% zQTMi = interp1(time, zQTMi,time);

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

% one = [1; 2];
% two = eye(2);
% three = one*two*one'

% dt = time(2)-time(1);
% B = [0 dt];
% % B = [B Vd 0*B];
% P = B*Q*B';

%%%%%%%%%%%%%%%%%%%% Translation Plotting %%%%%%%%%%%%%%%%%%%%
% startTime = 2.5;
% stopTime = 2.5;
startTime = 10;
stopTime = time(end);

transPlot = figure(Name="Plot of Translation comparison");

transIndicesZ = (zQTM ~= trans(3));
plot(time,zCV0, ...
    '.', 'Color',[109/255, 209/255, 255/255])
hold on
plot(t,zQTMi, '.k', MarkerSize=1)
% plot(time(transIndicesZ),zQTM(transIndicesZ), '.k'); %, MarkerSize=1)


%%%%%%%%%%%%%%%%%%%% Translation Plotting %%%%%%%%%%%%%%%%%%%%
% iter = 1;

timeTol = 0.1;
yPrev = zCV0(1);

x = [ zQTM(1); 0];

zKalman = zeros(length(t),1);

% Time-Varying Kalman Filter
for i = 2 : length(t)
    % if (zQTM(i) ~= trans(3))
       
        % curTime = time(i);                             % [seconds]

        % State Transition Matrix
        A = [ 1   dt ;
              0   1 ];
        % Control Input Matrix
        B = [ dt^2  ;
              dt ];

        % Find closest value
        for CViter = 1 : length(time)
            if (abs(time(CViter) - t(i)) < timeTol)
                y = zCV0(CViter);
                break
            elseif (i ~= 1)
                y = yPrev;
            end
        end
        % CViter = 1;
        % if (time(CViter))
            % Closest 
        % end

        % Initialization
        if i == 2
            % dt = time(i) - time(i-1);                  % [seconds]
            % B = [dt^2/2; dt];
            P = B*Q*B';
            % Acceleration
            % u = (zQTM(i) - zQTM(i-1))/(dt^2);          % [m/s^2]
            % x = [ zQTMi(i); (zQTMi(i) - zQTMi(i-1))/(dt) ];
        else
            % dt = curTime - prevTime;                   % [seconds]
            % u = ( zQTM(i) - zQTM(prevIter) ) / (dt^2); % [m/s^2]
        end

        u = (zQTMi(i) - zQTMi(i-1))/(dt^2);          % [m/s^2]

        % Current iteration variables
        % dt = time(i+1)-time(i);
        % y = zCV0(i);

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
    
        % Add points to list
        xKFestimate(i) = yEstimated;

        % iter = iter + 1;
        % prevTime = curTime;
        prevIter = i;
        yPrev = y;
    % end
end

plot(t, zKalman, '.r', MarkerSize=1)

[a, icons] = legend('zCV0', 'zQTMinterp', 'zKalman', 'Location','eastoutside');
% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

ylabel('z [m]')
xlabel('Time [seconds]')
xlim([startTime stopTime])
% ylim([-40 40])