import dronePosSingle_pb2
dp = dronePosSingle_pb2.dronePosition()
dp.x = 1.0
dp.y = 2.0
dp.z = 3.0
dp.dx = 1.1
dp.dy = 2.2
dp.dz = 3.3
dp.theta = 1.2
dp.phi = 2.2
dp.psi = 3.2
dp.dtheta = 1.3
dp.dphi = 2.3
dp.dpsi = 3.3

strdp = dp.SerializeToString()
print(strdp)
sizedp = dp.ByteSize()
print(sizedp)
