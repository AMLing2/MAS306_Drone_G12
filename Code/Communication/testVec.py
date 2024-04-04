import numpy as np
import dronePosVec_pb2

dp = dronePosVec_pb2.dronePosition()
dp.deviceType = 0;
dp.position[:] = [1.0,2.0,3.0] #[:] overwrites data
dp.positionDot[:] = [1.1,2.1,3.1]
dp.matrixSize[:] = [3,3]
rotMatrix = np.matrix('1.2 2.2 3.2; 4.2 5.2 6.2; 7.2 8.2 9.2')
rotMatrixDot = np.matrix('1.3 2.3 3.3; 4.3 5.3 6.3; 7.3 8.3 9.3')
#dp.rotMatrix.clear()
dp.rotMatrix[:] = rotMatrix.flat
dp.rotMatrixDot[:] = rotMatrixDot.flat
print(dp)
a = dp.SerializeToString()
print(a)
aSize = dp.ByteSize()
print(aSize)

dpRecv = dronePosVec_pb2.dronePosition()
dpRecv.ParseFromString(a)
print(dpRecv.deviceType)
print(dpRecv)
#convert back to numpy array:
rotMatrixNew = np.matrix(dpRecv.rotMatrix[:])
rotMatrixNew = rotMatrixNew.reshape(dpRecv.matrixSize[0],dpRecv.matrixSize[1])
print(rotMatrixNew)
