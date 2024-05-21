clc; clear; close all;

%% Computational Fluid Dynamics Simulation Result Plotting

radSec = [2900, 3000, 3100, 3200, 3300, ...
          3400, 3500, 3600, 3700, 3800]; % [rad/sec]

mk3_SG = [2.793, 2.958, 3.112, 3.275, 3.481, ...
          3.647, 3.876, 4.106, 4.256, 4.55]; % [N]
mk3_GG = [1.750, 1.850, 2.020, 2.110, 2.230, ...
          2.350, 2.490, 2.669, 2.746, 2.933]; % [N]

drone2024_SG = [2.39, 2.56, 2.71, 2.88, 3.06, ...
                3.30, 3.42, 3.59, 3.77, 4.03]; % [N]
drone2024_GG = [2.210, 2.360, 2.510, 2.670, 2.830, ...
                3.070, 3.160, 3.312, 3.485, 3.750]; % [N]

% Surface Goals
cfdSG = figure(Name='CDF Comparison: Surface Goals');

plot(radSec, mk3_SG, 'ok')
hold on
plot(radSec, drone2024_SG, 'rp')

xlim([2800 3900])
title('CFD Comparison: Surface Goals')
xlabel('Propeller Speed $\left[\frac{rad}{sec}\right]$', ...
        Interpreter='latex')
ylabel('Force [N]')
legend('2022 Design', '2024 Design', 'Location','southeast')

% Global Goals
cfdGG = figure(Name='CDF Comparison: Global Goals');

plot(radSec, mk3_GG, 'ok')
hold on
plot(radSec, drone2024_GG, 'rh')

xlim([2800 3900])
title('CFD Comparison: Global Goals')
xlabel('Propeller Speed $\left[\frac{rad}{sec}\right]$', ...
        Interpreter='latex')
ylabel('Force [N]')
legend('2022 Design', '2024 Design', 'Location','southeast')
