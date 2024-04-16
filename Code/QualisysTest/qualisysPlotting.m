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
v0R11 = data(:,8);
v0R12 = data(:,9);
v0R13 = data(:,10);
v0R21 = data(:,11);
v0R22 = data(:,12);
v0R23 = data(:,13);
v0R31 = data(:,14);
v0R32 = data(:,15);
v0R33 = data(:,16);
v1R11 = data(:,17);
v1R12 = data(:,18);
v1R13 = data(:,19);
v1R21 = data(:,20);
v1R22 = data(:,21);
v1R23 = data(:,22);
v1R31 = data(:,23);
v1R32 = data(:,24);
v1R33 = data(:,25);


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


figure(Name="Rotation comparison")
sgtitle("Rotation Comparison")

subplot(3,3,1)
plot(time, v0R11)
hold on
plot(time, v1R11)

subplot(3,3,2)
plot(time, v0R12)
hold on
plot(time, v1R12)

subplot(3,3,3)
plot(time, v0R13)
hold on
plot(time, v1R13)

subplot(3,3,4)
plot(time, v0R21)
hold on
plot(time, v1R21)

subplot(3,3,5)
plot(time, v0R22)
hold on
plot(time, v1R22)

subplot(3,3,6)
plot(time, v0R23)
hold on
plot(time, v1R23)

subplot(3,3,7)
plot(time, v0R31)
hold on
plot(time, v1R31)

subplot(3,3,8)
plot(time, v0R32)
hold on
plot(time, v1R32)

subplot(3,3,9)
plot(time, v0R33)
hold on
plot(time, v1R33)
