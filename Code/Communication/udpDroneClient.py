import dronePosVec_pb2
import socket
import time
import numpy as np

localIP = "127.0.0.1"
localPort = 20001
dp = dronePosVec_pb2.dronePosition()

class clientSocket:
	buffersize = 1024
	def __init__(self):
		self.sock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM) #unsure if using AF_INET
		#self.sock.bind((localIP,localPort))

	def connect(self,addressPort):
		self.sock.connect(addressPort)

	def gethostname(self):#kind of useless, remove later
		self.sock.getsockname()
	
	def initSend(self,address,msg):
		self.sock.sendto(msg,address)

	def send(self,msg):
		self.sock.send(msg)		

def pbInit():
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


pbstring = pbInit()
droneSocket = ClientSocket()
#mysocket.connect(localIP,localPort)

print(droneSocket.gethostname)
#clientRecieve = mysocket.getclient()
#print(clientRecieve)
#clientAddress = clientRecieve[1]
#mysocket.connect(clientAddress)
serverAddress = ("128.39.200.239",20002)
droneSocket.initSend(serverAddress,str.encode("hi"))

i = 0
while(0):
	print("msg sent")
	mysocket.send(str.encode("hi"))
	i += 1
	time.sleep(10)
