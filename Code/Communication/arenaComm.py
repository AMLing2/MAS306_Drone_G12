import dronePosVec_pb2
import socket
import time #unsure if needed, might remove later
import numpy as np
from threading import Thread
import multiprocessing as mp

localIP = "127.0.0.1"
publicIP = "128.39.200.239" #please automate this...
localPort = 20001
dp = dronePosVec_pb2.dronePosition()

class ServSocket:
	buffersize = 1024
	def __init__(self,IP,port):
		self.sockIP = IP
		self.port = port
		self.sock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM) #unsure if using AF_INET
		self.sock.bind((self.localIP,self.port))

	def connect(self,addressPort):
		self.sock.connect(addressPort)

	def gethostname(self):
		self.sock.getsockname()

	def getclient(self):
		return self.sock.recvfrom(self.buffersize)

	def send(self,msg):
		self.sock.send(msg)		

def DroneServer(q):
	dServSock = ServSocket(publicIP,20002)
	droneAddress = None
	print("listening to drone input on: " + str(dServSock.sockIP)+":" + str(dServSock.port))
	try:
		dServSock.sock.settimeout(10.0) #must be called before recv or it might not work
		droneAddress = (dServSock.getclient())[1]
	except:
	 	print("timeout 1")
	if droneAddress == None:
		print("failed to get data from drone")
	else:
		print("drone address: "+str(droneAddress))
		print("connecting to drone")
		dServSock.connect(droneAddress)
	q.put("hi")

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

#--------------MAIN PROCESS-------------------@
if __name__ == '__main__'	:
	print("starting drone comm process")
	time_start = time.time()
	q = mp.Queue()
	pDrone = mp.Process(target=DroneServer,args=(q,))
	pDrone.start()
	time.sleep(15)
	print(q.get())
	print("closing drone server process")
	print("time: " + str(time.time() - time_start))
	pDrone.join()

if False: #remove later, used as block comment
	pbstring = pbInit()
	mysocket = ServSocket(localIP,localPort)
	#mysocket.connect(localIP,localPort)

	print(mysocket.gethostname)
	clientRecieve = mysocket.getclient()
	print(clientRecieve)

	clientAddress = clientRecieve[1]
	mysocket.connect(clientAddress)
	i = 0
	while(1):
		print("msg sent")
		mysocket.send(pbstring)
		i += 1
		time.sleep(10)
