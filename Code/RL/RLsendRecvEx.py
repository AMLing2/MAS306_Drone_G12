#from ..Modules.py import arenaComm
from Modules import arenaComm
from Modules import dronePosVec_pb2
import math
import time

hostname = 'b12.uia.no'
messager = arenaComm.arenaCommunication(arenaComm.SendrecvType.GENERICRECV | arenaComm.SendrecvType.GENERICSEND,hostname,{1,3},dronePosVec_pb2.rl)

msg = None
motorvals = 0
running = True
dp = dronePosVec_pb2.dronePosition()

try:
    while running:
        if messager.qRecv.empty() == False:
            msg = messager.qRecv.get()
            dp.ParseFromString(msg)
            print("msg recieved with type: " + str(dp.deviceType))

        #
        # PYTORCH COULD GO HERE FOR EXAMPLE
        #
        messager.dc.Clear()
        motorvals = math.sin(math.pi*2*((time.time()%20)/20)) #make a sine wave function with a period of 20s
        messager.dc.motorFL = motorvals
        messager.dc.motorFR = motorvals
        messager.dc.motorBL = motorvals
        messager.dc.motorBR = motorvals
        messager.dc.killswitch = False
        messager.addMotToSendQueue()
except KeyboardInterrupt: #exit with ctrl+c
    pass

#end multiprocesses
print("\n ending program")
messager.endmps()