clc; clear; close all;

%% Kalman Filter

g = 9.80665;    % [m/s^2]
m = 0.250;      % [kg]
d = 0.0;        % [m]
Ix = 340826e-9; % [kg*m^2]
Iy = 886422e-9; % [kg*m^2]
Iz = 602980e-9; % [kg*m^2]
c = 0.0;        % unit?

% State Transition Matrix
A = [ 0   1  ;
      0   0 ];
% Control Input Matrix
B = [   0     0     0     0   ;
        0     0     0     0   ;
        0     0     0     0   ;
        0     0     0     0   ;
        0     0     0     0   ;
      -1/m  -1/m  -1/m  -1/m  ;
        0     0     0     0   ;
        0     0     0     0   ;
        0     0     0     0   ;
        0    d/Ix   0  -d/Ix  ;
       d/Iy   0   -d/Iy   0   ;
      -c/Iz  c/Iz -c/Iz c/Iz ];
% Measurement Matrix
C = [ 1 0 0 0 0 0 0 0 0 0 0 0  ;
      0 1 0 0 0 0 0 0 0 0 0 0  ;
      0 0 1 0 0 0 0 0 0 0 0 0  ;
      0 0 0 0 0 0 1 0 0 0 0 0  ;
      0 0 0 0 0 0 0 1 0 0 0 0  ;
      0 0 0 0 0 0 0 0 1 0 0 0 ];
% Feedthrough Matrix
D = zeros(6,4);