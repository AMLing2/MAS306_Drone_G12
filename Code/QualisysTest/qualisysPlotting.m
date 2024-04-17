clc; clear; close all;

%% Plotting results Qualisys Test 

data = csvread("ExportedResults_0.csv", 2,0);

time = data(:,1);
xQTM = data(:,2);
yQTM = data(:,3);
zQTM = data(:,4);
xCV = data(:,5);
yCV = data(:,6);
zCV = data(:,7);

%%%%%%%%%%%%%%%%%%%%%% Translation Plotting %%%%%%%%%%%%%%%%%%%%%%
figure(Name="Plot of Translation comparison")

% X plotting
sgtitle("Translation Comparison")
subplot(3,1,1)
plot(time,xCV, '.r')
hold on
plot(time,xQTM, '.k', MarkerSize=1)
ylabel('x [m]')
xlabel('Time [seconds]')
[a, icons] = legend('xCV', 'xQTM', 'Location','eastoutside');
% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

% Y plotting - Remember to flip sign, only axis which is aligned
subplot(3,1,2)
plot(time,-yCV, '.g')
hold on
plot(time,yQTM, '.k', MarkerSize=1)
ylabel('y [m]')
xlabel('Time [seconds]')
[a, icons] = legend('yCV', 'yQTM', 'Location','eastoutside');
% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

% Z plotting
subplot(3,1,3)
plot(time,zCV, '.b')
hold on
plot(time,zQTM, '.k', MarkerSize=1)
ylabel('z [m]')
xlabel('Time [seconds]')
[a, icons] = legend('zCV', 'zQTM', 'Location','eastoutside');
% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

%%%%%%%%%%%%%%%%%%%%%% Rotation Plotting %%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%% Rotation Vec0 and Matrix %%%%%%%%%%%%
figure(Name="Rotation comparison0")
sgtitle("Rotation Matrix Comparison: Vec0 and QTM")

% Plotting rotVector[0] with QTM
for i = 1:9
    subplot(3, 3, i);
    
    vec0 = data(:, i+7); % v0Rij
    QTM = data(:, i+25); % v1Rij
    
    if any(i == [1,4,7])
        plot(time, vec0, '.r')
    elseif any(i == [2,5,8])
        plot(time, vec0, '.g')
    else
        plot(time, vec0, '.b')
    end
    hold on
    plot(time, QTM, '.k', MarkerSize=1)
    
    xlabel('Time [seconds]');
    title(['Element ', num2str(i)])
    hold off
    xlim([25 125])
    ylim([-1 1])
end

% Add a bit space to the figure
fig = gcf;
fig.Position(3) = fig.Position(3) + 250;

% Add common legend outside subplots
[lgd, icons] = legend('Vec0', 'QTM');
lgd.Position(1) = 0.01;
lgd.Position(2) = 0.4;

% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)
%%%%%%%%%%%% Rotation Vec0 and Matrix diff plot %%%%%%%%%%%%
figure(Name="RotationDifference0")
sgtitle("Rotation Matrix Element Difference: Vec0 - QTM")

% Plotting rotVector[0] with QTM
for i = 1:9
    subplot(3, 3, i);
    
    vec0 = data(:, i+7); % v0Rij
    QTM = data(:, i+25); % v1Rij
    
    % Plot only when QTM rotation matrix != 0
    validIndices = (QTM ~= 0);
    plot(time(validIndices), (vec0(validIndices)-QTM(validIndices)), ...
         '.k', MarkerSize=1)
    
    xlabel('Time [seconds]');
    title(['Element ', num2str(i)])
    hold off
    xlim([25 125])
    ylim([-1 1])
end

% Add a bit space to the figure
fig = gcf;
fig.Position(3) = fig.Position(3) + 250;

% Add common legend outside subplots
[lgd, icons] = legend('Vec0-QTM');
lgd.Position(1) = 0.01;
lgd.Position(2) = 0.4;

% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

%%%%%%%%%%%% Rotation Vec1 and Matrix %%%%%%%%%%%%
figure(Name="Rotation Comparison1")
sgtitle("Rotation Matrix Comparison: Vec1 and QTM")

% Plotting rotVector[1] with QTM
for i = 1:9
    subplot(3, 3, i);
    
    vec1 = data(:, i+16); % v0Rij
    QTM = data(:, i+25); % v1Rij
    
    if any(i == [1,4,7])
        plot(time, vec1, '.r')
    elseif any(i == [2,5,8])
        plot(time, vec1, '.g')
    else
        plot(time, vec1, '.b')
    end
    hold on
    plot(time, QTM, '.k', MarkerSize=1)
    
    xlabel('Time [seconds]');
    title(['Element ', num2str(i)])
    hold off
    xlim([25 125])
    ylim([-1 1])
end

% Add a bit space to the figure
fig = gcf;
fig.Position(3) = fig.Position(3) + 250;

% Add common legend outside subplots
[lgd, icons] = legend('Vec1', 'QTM');
lgd.Position(1) = 0.01;
lgd.Position(2) = 0.4;

% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

%%%%%%%%%%%% Rotation Vec1 and Matrix diff plot %%%%%%%%%%%%
figure(Name="RotationDifference1")
sgtitle("Rotation Matrix Element Difference: Vec1 - QTM")

% Plotting rotVector[1] with QTM
for i = 1:9
    subplot(3, 3, i);
    
    vec1 = data(:, i+16); % v0Rij
    QTM = data(:, i+25); % v1Rij
    
    % Plot only when QTM rotation matrix != 0
    validIndices = (QTM ~= 0);
    plot(time(validIndices), (vec1(validIndices)-QTM(validIndices)), ...
         '.k', MarkerSize=1)
    
    xlabel('Time [seconds]');
    title(['Element ', num2str(i)])
    hold off
    xlim([25 125])
    ylim([-1 1])
end

% Add a bit space to the figure
fig = gcf;
fig.Position(3) = fig.Position(3) + 250;

% Add common legend outside subplots
[lgd, icons] = legend('Vec1-QTM');
lgd.Position(1) = 0.01;
lgd.Position(2) = 0.4;

% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

%%%%%%%%%%% 
figure(Name="Angle")
hold on
angles0 = zeros(length(time), 1);
for i = 1 : length(time)
    rotMat0 = [data(i,8), data(i,9), data(i,10);
               data(i,11), data(i,12), data(i,13);
               data(i,14), data(i,15), data(i,16)];
    rotMatQTM = [data(i,26), data(i,27), data(i,28);
                 data(i,29), data(i,30), data(i,31);
                 data(i,32), data(i,33), data(i,34)];
    rotMatDiff = rotMat0' * rotMatQTM;
    rotVecDiff = rotm2axang(rotMatDiff);
    angles0(i) = rad2deg(rotVecDiff(4));
end

% Plot only when QTM rotation matrix != 0
validIndices = (QTM ~= 0);
plot(time(validIndices), angles0(validIndices), '.k', MarkerSize=1)
title("Difference from Axis-Angle")
ylabel("Angle [degrees]")
xlabel("Time [seconds]")