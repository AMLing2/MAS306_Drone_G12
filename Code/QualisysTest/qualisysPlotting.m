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