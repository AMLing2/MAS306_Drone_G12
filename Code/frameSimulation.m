clc; clear; close all;
% Reference Frame Simulation

%% ================== CAMERA ORIGIN ================== 

height = 1;
sides = 0.9;

% ------ Rotation matrices ------------
% Angles [radians]
a = pi/2;
b = pi;
c = 0;

yaw = [cos(a), -sin(a), 0;
       sin(a),  cos(a), 0;
       0,         0,    1];

pitch = [cos(b), 0, sin(b);
         0, 1, 0;
         -sin(b), 0, cos(b)];

roll = [1, 0, 0;
        0, cos(c), -sin(c);
        0, sin(c), cos(c)];
rotMat = yaw*pitch*roll;
% ------ Rotation matrices ------------

arenaCenter = [0 0 height];
arenaOrientation = eye(3)*rotMat;
camera = [0 0 0];
cameraOrientation = eye(3);

% Translation Drone
tVec = [-0.1973 -0.0402  0.5268];
tVec = [-0.0034  0.02    0.6376];
% Orientation Drone
droneMat = [ 0.99258427  0.12052626  0.01580802;
             0.1150138  -0.88907301 -0.44308126;
            -0.03934845  0.44161363 -0.89634207];
droneMat = [ 0.99460037  0.05461334 -0.08824673;
             0.0357113  -0.97851045 -0.20308127;
            -0.09744129  0.1988333  -0.97517717];

figure(Name="CameraOrigin")

% Plot Camera
poseplot(cameraOrientation, camera, ScaleFactor=0.05)
hold on

% Plot Arena Center
poseplot(arenaOrientation, arenaCenter, ScaleFactor=0.05)

% Plot Drone ref Camera
poseplot(droneMat, tVec, ScaleFactor=0.05)

% Drone ref Arena
camera_arena = arenaCenter - camera;
arena_drone = tVec - camera_arena;
arena_droneOrientation = arenaOrientation*droneMat;
% Plot Drone ref Arena
% poseplot(arena_droneOrientation, (arena_drone+camera_arena), ScaleFactor=0.05)

% Box will be 30x30x120 [cm] for simulation
xlim([-sides/2 sides/2])
ylim([-sides/2 sides/2])
zlim([-0.1 height+0.1])

legend("Camera", "Arena Center", "Drone ref Camera", "Drone ref Arena")
title("Top/Origin = Camera")
xlabel("X-axis")
ylabel("Y-axis")
zlabel("Z-axis")