clc; clear; close all;
% Reference Frame Simulation

%% ================== CAMERA ORIGIN ================== 

height = 1;
sides = 0.9;

% ------ Rotation matrices ------------
% Angles [radians]
a = 0;
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
% ------ Rotation matrices ------------

arenaCenter2 = [0 0 height];
arenaOrientation2 = eye(3)*pitch;
camera2 = [0 0 0];
cameraOrientation2 = eye(3);

% Translation Drone
tVec = [-0.1973 -0.0402  0.5268];
% Orientation Drone
droneMat2 = [ 0.99258427  0.12052626  0.01580802;
             0.1150138  -0.88907301 -0.44308126;
            -0.03934845  0.44161363 -0.89634207];

figure(Name="CameraOrigin")

% Plot Camera
poseplot(cameraOrientation2, camera2, ScaleFactor=0.05)
hold on

% Plot Arena Center
poseplot(arenaOrientation2, arenaCenter2, ScaleFactor=0.05)

% Plot Drone
poseplot(droneMat2, tVec, ScaleFactor=0.05)

% Box will be 30x30x120 [cm] for simulation
xlim([-sides/2 sides/2])
ylim([-sides/2 sides/2])
zlim([-0.1 height+0.1])

legend("Camera", "Arena Center", "Drone")
title("Top/Origin = Camera")
xlabel("X-axis")
ylabel("Y-axis")
zlabel("Z-axis")

hold off

%% ================== ARENA ORIGIN ================== 

height = 1;
sides = 0.9;

% ------ Rotation matrices ------------
% Angles [radians]
a = 0;
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
% ------ Rotation matrices ------------

arenaCenter2 = [0 0 0];
arenaOrientation2 = eye(3);
camera2 = [0 0 height];
cameraOrientation2 = eye(3)*pitch;

tVec = [-0.1973 -0.0402  0.5268];

camera_drone2 = tVec - camera2;
camera_arena2 = arenaCenter2 - camera2;
arena_drone2 = camera_drone2 - camera_arena2;

% Convert the rotation vector to a rotation matrix
droneMat2 = [ 0.99258427  0.12052626  0.01580802;
             0.1150138  -0.88907301 -0.44308126;
            -0.03934845  0.44161363 -0.89634207];
droneOrientation2 = droneMat2 * pitch;

figure(Name="ArenaOrigin")

% Plot Camera
poseplot(cameraOrientation2, camera2, ScaleFactor=0.05)
hold on

% Plot Arena Center
poseplot(arenaOrientation2, arenaCenter2, ScaleFactor=0.05)

% Plot Drone
poseplot(droneOrientation2, arena_drone2, ScaleFactor=0.05)

% Box will be 30x30x120 [cm] for simulation
xlim([-sides/2 sides/2])
ylim([-sides/2 sides/2])
zlim([-0.1 height+0.1])

legend("Camera","Arena Center","Drone")
title("Top/Origin = Arena Center")
xlabel("X-axis")
ylabel("Y-axis")
zlabel("Z-axis")

% Reverse Z-axis numbers
zt = get(gca, 'ZTick');
set(gca, 'ZTick',zt, 'ZTickLabel',fliplr(zt))
