import dronePosVec_pb2
import socket
import time
import numpy as np
from threading import Thread
import queue

localIP = "127.0.0.1"
localPort = 20001
dp = dronePosVec_pb2.dronePosition()
dt = dronePosVec_pb2.dataTransfers()

class ClientSocket:
	buffersize = 1024
	def __init__(self):
		self.sock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM) #unsure if using AF_INET
		#self.sock.bind((localIP,localPort))

	def connect(self,addressPort):
		self.sock.connect(addressPort)

	def gethostname(self):#kind of useless, remove later
		self.sock.getsockname()

	def initRecv(self):
		return self.sock.recvfrom(self.buffersize)

	def initSend(self,address,msg):
		self.sock.sendto(msg,address)

	def send(self,msg):
		self.sock.send(msg)		

	def recv(self):
		return self.sock.recv(self.buffersize)

def pbInit():
	dt.ID = 2 #2 = drone
	dt.msg = "msg"
	return dt.SerializeToString()

def pbData(): #temp test
	dp.deviceType = 0;
	dp.position[:] = [1.0,2.0,3.0] #[:] overwrites data
	dp.positionDot[:] = [1.1,2.1,3.1]
	dp.matrixSize[:] = [3,3]
	rotMatrix = np.matrix('1.2 2.2 3.2; 4.2 5.2 6.2; 7.2 8.2 9.2')
	rotMatrixDot = np.matrix('1.3 2.3 3.3; 4.3 5.3 6.3; 7.3 8.3 9.3')
	#dp.rotMatrix.clear()
	dp.rotMatrix[:] = rotMatrix.flat
	dp.rotMatrixDot[:] = rotMatrixDot.flat
	return dp.SerializeToString() 

def motorListen(droneSocket,qMotor):
	print("motorListen thread up")
	droneSocket.sock.settimeout(None)
	while(True):
		try:
			qMotor.put(droneSocket.recv())
		except:
			print("motor reciever timed out")
			qMotor.put(0)
			break

def sendIMU(droneSocket):
	print("sendIMU thread up")
	while(True):
		msgString = pbData()
		droneSocket.send(msgString)

#-----------------MAIN----------------@
pbstring = pbInit()
droneSocket = ClientSocket()

serverAddress = ("128.39.200.239",20002)
i = 0
stringRecv = None
while(True):
	droneSocket.initSend(serverAddress,pbstring)
	try:
		droneSocket.sock.settimeout(3.0)
		stringRecv = droneSocket.initRecv()
		print("string recieved: " + str(stringRecv))
	except:
		print("timeout, retrying " + str(i))
		i += 1
	if not stringRecv == None:
	 break

print("connecting to:" + str(stringRecv[1]))
droneSocket.connect(stringRecv[1])
droneSocket.send(pbstring)
qMotor = queue.Queue(5)

tMotorRecv = Thread(target=motorListen, args=((droneSocket,qMotor)))
tIMUsend = Thread(target = sendIMU, args=(droneSocket,))
tMotorRecv.run()
tIMUsend.run()
print("sleeping")
i = 0
while(i<=30):
	print(qMotor.get())
	time.sleep(1)
	i += 1
print("joining threads")
tMotorRecv.join()
tIMUsend.join()

if False:	
	print("echoing")
	droneSocket.sock.settimeout(10)
	while(True):
		msg = droneSocket.recv()
		print("recieved: " + str(msg))
		droneSocket.send(str.encode("Recv: " + str(msg)))
