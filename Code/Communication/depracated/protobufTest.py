import dronePos_pb2
position = dronePos_pb2.position()
position.x = 1.0
position.y = 2.0
position.z = 3.0

positionDot = dronePos_pb2.positionDot()
positionDot.dx = 1.1
positionDot.dy = 2.1
positionDot.dz = 3.1

rotation = dronePos_pb2.rotation()
rotation.theta = 1.2
rotation.phi = 2.2
rotation.psi = 3.2

rotationDot = dronePos_pb2.rotationDot()
rotationDot.dtheta = 1.3
rotationDot.dphi = 2.3
rotationDot.dpsi = 3.3

strPosition = position.SerializeToString()
print(strPosition)
print(position.ByteSize())
