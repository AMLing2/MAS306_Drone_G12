#from ..Modules.py import arenaComm
from Modules import arenaComm
from Modules import dronePosVec_pb2
import math
import time

reciever = arenaComm.arenaRecvPos(True) #true = using generic reciever
sender = arenaComm.droneSender(True)



motorvals = 0
running = True
while running:
    sender.dc.Clear()
    motorvals = math.sin(math.pi*2*((time.time()%20)/20)) #make a sine wave function with a period of 20s
    sender.dc.motorFL,sender.dc.motorFR,sender.dc.motorBL,sender.dc.motorBR = motorvals
    sender.addPosToQueue()

#end multiprocesses
reciever.endRecv()
sender.endRecv()