clc; clear; close all;
% Reference Frame Simulation

%% ================== CAMERA ORIGIN ================== 

height = 1;
sides = 0.9;

% ------ Quaternion Variables -------

% Drone Translation Vector from OpenCV
tVec = [-0.0034  0.02    0.6376];

% Drone Rotation Vector from OpenCV
rVec = [ 2.93496616  0.06714296 -0.13803149];
droneMat = [ 0.99460037  0.05461334 -0.08824673;
             0.0357113  -0.97851045 -0.20308127;
            -0.09744129  0.1988333  -0.97517717];

% Axis of rotation
angle = sqrt(sum(rVec.^2));
normVec = rVec/angle;

% Quaternion
q1 = quaternion(droneMat, 'rotmat', 'frame');

% Rotation Quaternion
a = pi; % Angle [rad]
v = [-1 1 0];
v = v/ (sqrt(sum(v.^2)));
v = v*a;

m = rotvec2mat3d(v);

q2 = quaternion(m, 'rotmat', 'frame');

q = q1*q2

% Quaternions
% q1 =  cosd(a) + sind(a)  * qVec;
% q2 = cosd(-a) + sind(-a) * qVec;


% ------ Quaternion Variables -------

% arenaCenter = [0 0 height];
% arenaOrientation = eye(3);
camera = [0 0 0];
cameraOrientation = eye(3);

% Orientation Drone
% droneMat = [ 0.99258427  0.12052626  0.01580802;
%              0.1150138  -0.88907301 -0.44308126;
%             -0.03934845  0.44161363 -0.89634207];

figure(Name="CameraOrigin")

% Plot Camera
poseplot(cameraOrientation, camera, ScaleFactor=0.05)
hold on

% Plot Arena Center
% poseplot(arenaOrientation, arenaCenter, ScaleFactor=0.05)

% Plot Drone ref Camera
poseplot(q, tVec, ScaleFactor=0.05)
poseplot(droneMat, tVec, ScaleFactor=0.05)

% Drone ref Arena
% camera_arena = arenaCenter - camera;
% arena_drone = tVec - camera_arena;
% arena_droneOrientation = arenaOrientation*droneMat;
% Plot Drone ref Arena
% poseplot(arena_droneOrientation, (arena_drone+camera_arena), ScaleFactor=0.05)

% Box will be 30x30x120 [cm] for simulation
xlim([-sides/2 sides/2])
ylim([-sides/2 sides/2])
zlim([-0.1 height+0.1])

% legend("Camera", "Arena Center", "Drone ref Camera", "Drone ref Arena")
title("Top/Origin = Camera")
xlabel("X-axis")
ylabel("Y-axis")
zlabel("Z-axis")