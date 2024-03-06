clc; clear; close all;

%% Reference Frame Simulation

arenaCenter = [0.0 0.0 0.0];
camera = [0.0 0.0 1.2];

rVec = [2.39290834, 0.6210523, -0.26670166];
tVec = [-0.2497, -0.0591, 0.8337];

camera_drone = tVec - camera;
camera_arena = arenaCenter - camera;
arena_drone = camera_drone - camera_arena;

% Convert the rotation vector to a rotation matrix
%R_drone_to_cam = [ 0.99410564  0.10825669  0.00587067;
 %                  0.08566473 -0.75115132 -0.65454812;
  %                -0.06644945  0.65119289 -0.75599755];

% Plot Arena center & Camera
plot3(arenaCenter(1), arenaCenter(2), arenaCenter(3), 'ko')
hold on; grid on
plot3(camera(1), camera(2), camera(3), 'kp')

plot3([arenaCenter(1) arena_drone(1)], [arenaCenter(2) arena_drone(2)], ...
      [arenaCenter(3) arena_drone(3)], 'p-r')

% Box will be 30x30x120 [cm] for simulation
xlim([-0.15 0.15])
ylim([-0.15 0.15])
zlim([0 1.2])

