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

%%%%%%%%%%% Translation Plotting %%%%%%%%%%%
figure(Name="Plot of Translation comparison")

% X plotting
sgtitle("Translation Comparison")
subplot(3,1,1)
plot(time,xQTM, '.r')
hold on
plot(time,xCV, '.k', MarkerSize=1)
ylabel('x [m]')
xlabel('Time [seconds]')
[a, icons] = legend('xQTM', 'xCV', 'Location','eastoutside');
% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

% Y plotting - Remember to flip sign, only axis which is aligned
subplot(3,1,2)
plot(time,yQTM, '.g')
hold on
plot(time,-yCV, '.k', MarkerSize=1)
ylabel('y [m]')
xlabel('Time [seconds]')
[a, icons] = legend('yQTM', 'yCV', 'Location','eastoutside');
% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

% Z plotting
subplot(3,1,3)
plot(time,zQTM, '.b')
hold on
plot(time,zCV, '.k', MarkerSize=1)
ylabel('z [m]')
xlabel('Time [seconds]')
[a, icons] = legend('zQTM', 'zCV', 'Location','eastoutside');
% Change size of legend icons
icons = findobj(icons, '-property', 'Marker', '-and', '-not', 'Marker', 'none');
set(icons, 'MarkerSize', 20)

%%%%%%%%%%% Rotation Plotting %%%%%%%%%%%

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
    hold on;
    plot(time, QTM, '.k', MarkerSize=1)
    
    xlabel('Time [seconds]');
    title(['Element ', num2str(i)])
    hold off;
    xlim([12.9 27])
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
    hold on;
    plot(time, QTM, '.k', MarkerSize=1)
    
    xlabel('Time [seconds]');
    title(['Element ', num2str(i)])
    hold off;
    xlim([12.9 27])
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