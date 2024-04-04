import numpy as np
import dronePosVec_pb2
import socket

localIP = "127.0.0.1"
localPort = 20001 #change
bufferSize = 1024

dp = dronePosVec_pb2.dronePosition()
dp.position[:] = [1.0,2.0,3.0] #temp, just as test
sendBytes = dp.SerializeToString()
print(sendBytes)

#create datagram socket
s = socket.socket(socket.AF_INET,socket.SOCK_DGRAM) #SOCK_DGRAM = udp

#bind to address and ip
s.bind((localIP, localPort))

print("udp server up")

#pcIP = "128.39.203.209"

s.connect((localIP,localPort))

s.send(sendBytes)
