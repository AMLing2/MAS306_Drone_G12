clc; clear; close all;

%% Plotting results Qualisys Test 

data = csvread("ExportedResults_12.csv", 2,0);

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
plot(time,xQTM)
hold on
plot(time,xCV)
legend('xQTM', 'xCV')
ylabel('x [m]')
xlabel('Time [seconds]')

% Y plotting - Remember to flip sign, only axis which is aligned
subplot(3,1,2)
plot(time,yQTM)
hold on
plot(time,-yCV)
legend('yQTM', 'yCV')
ylabel('y [m]')
xlabel('Time [seconds]')

% Z plotting
subplot(3,1,3)
plot(time,zQTM)
hold on
plot(time,zCV)
legend('zQTM', 'zCV')
ylabel('z [m]')
xlabel('Time [seconds]')

%%%%%%%%%%% Rotation Plotting %%%%%%%%%%%

figure(Name="Rotation comparison0")
sgtitle("Rotation Matrix Comparison: Vec0 and QTM")

% Plotting rotVector[0] with QTM
for i = 1:9
    subplot(3, 3, i);
    
    vec0 = data(:, i+7); % v0Rij
    QTM = data(:, i+25); % v1Rij
    
    if any(i == [3, 6, 7, 8])
        plot(time, abs(vec0))
    else
        plot(time, vec0)
    end
        hold on;
        plot(time, QTM)
    
    xlabel('Time [seconds]');
    %ylabel(['Value of v' num2str(floor((i-1)/3)+1) 'R' num2str(mod(i-1,3)+1)])
    title(['Element ', num2str(i)])
    legend('Vec0', 'QTM');
    hold off;
end

figure(Name="Rotation Comparison1")
sgtitle("Rotation Matrix Comparison: Vec1 and QTM")

% Plotting rotVector[1] with QTM
for i = 1:9
    subplot(3, 3, i);
    
    vec1 = data(:, i+16); % v0Rij
    QTM = data(:, i+25); % v1Rij
    
    plot(time, vec0)
    hold on;
    plot(time, QTM)
    
    xlabel('Time [seconds]');
    %ylabel(['Value of v' num2str(floor((i-1)/3)+1) 'R' num2str(mod(i-1,3)+1)])
    title(['Element ', num2str(i)])
    legend('Vec1', 'QTM');
    hold off;
end