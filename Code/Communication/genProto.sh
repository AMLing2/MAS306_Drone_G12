echo "generating protobuf files"
protoc -I=. --python_out=. --python_out=../Camera --python_out=../RL/Modules ./dronePosVec.proto
protoc -I=. --cpp_out=../Drone/droneIMU/src --cpp_out=./cpp/arenaServer/src ./dronePosVec.proto
