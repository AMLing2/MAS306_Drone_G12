import dronePosVec_pb2
import socket
import time
import numpy as np
import threading as tr
import queue
import multiprocessing as mp

localIP = "127.0.0.1"
localPort = 20001
dp = dronePosVec_pb2.dronePosition()
dataMsg = dronePosVec_pb2.dataTransfers()

class ClientSocket:
	motorRead = 0 #would be better as an enum, gives method to break thread externally
	imuSend = 0 
	globalTimer = None
	timerOffset = 0 #unsure how good this is
	buffersize = 1024
	def __init__(self,serverAddress):
		self.serverAddress = serverAddress
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

	def dserverConnect(self):
		stringRecv = None
		for i in range(10):
			self.initSend(self.serverAddress,pbstring) #pbstring should be more local
			try:
				self.sock.settimeout(3.0)
				stringRecv = self.initRecv() #inital send
				print("string recieved: " + str(stringRecv))
			except:
				print("timeout, retrying " + str(i))
			if not stringRecv == None:
			 break
		if stringRecv == None:
			return 1 #failure to send
		print("connecting to:" + str(stringRecv[1]))
		self.connect(stringRecv[1])
		self.send(pbstring) #connection msg
		return 0 #sucess

	def sSyncTimer(self,dataMsg): #sync local timer from server
		dataMsg.Clear()
		droneSocket.sock.settimeout(None)
		dataMsg.ParseFromString(self.recv())
		print("globalTImer: " +str(dataMsg.timeSync_ns))
		self.globalTimer = dataMsg.timeSync_ns
		syncInterval = 100000000 #100ms
		sleepLen = sleepTimeCalc(syncInterval,globalTimer)
		time.sleep(sleepLen) #sleep until sync time
		responseTime = time.time_ns()
		print("responseTime: " + str(responseTime))
		
		dataMsg.Clear()
		dataMsg.ID = 2 #for drone probably
		dataMsg.timeSync_ns = responseTime
		dataMsg.msg = "responseTime" #probably ignored
		self.send(dataMsg.SerializeToString())

		dataMsg.ParseFromString(self.recv())
		self.offset = dataMsg.timeSync_ns
		self.globalTimer = self.globalTimer + self.offset
		print("offset: " + str(self.offset))

class TimeTracker:
	def __init__(globalTimer,qAction):
		self.globalTimer = globalTimer
#self.qAction = 


def sleepTimeCalc(interval,timer):
	return float(interval - ((time.time_ns() - timer) % interval))/1000000000.0

def pbInit(): #remove
	dataMsg.ID = 2 #2 = drone
	dataMsg.msg = "msg"
	return dataMsg.SerializeToString()

def pbData(): #temp test
	dp.deviceType = 0;
	dp.position[:] = [1.0,2.0,3.0] #[:] overwrites data
	dp.positionDot[:] = [1.1,2.1,3.1]
	dp.matrixSize[:] = [3,3]
	rotMatrix = np.matrix('1.2 2.2 3.2; 4.2 5.2 6.2; 7.2 8.2 9.2')
	rotMatrixDot = np.matrix('1.3 2.3 3.3; 4.3 5.3 6.3; 7.3 8.3 9.3')
	#dp.rotMatrix.Clear()
	dp.rotMatrix[:] = rotMatrix.flat
	dp.rotMatrixDot[:] = rotMatrixDot.flat
	return dp.SerializeToString() 


#def pTimeTracker(globalTimer,qAction):
	#if time.time_ns() >= 

def motorListen(droneSocket,qMotor): #remove
	print("motorListen thread up")
	while(True):
		time.sleep(10)
		if droneSocket.motorRead == 2:
			break
		elif droneSocket.motorRead == 1:	#temporary								 
			try:
				droneSocket.sock.settimeout(3.0)
				print(droneSocket.recv()) # do something with this, put in queue
				droneSocket.motorRead = 0 #?
			except:
				print("motor reciever timed out")
	print("motorListen thread done")

def sendIMU(droneSocket): #remove
	print("sendIMU thread up")
	while(True):
		if droneSocket.imuSend == 2:
			break
		elif droneSocket.imuSend == 1: #temporary, should be more in thread only
			droneSocket.imuSend = 0 #?
			msgString = pbData()
			droneSocket.send(msgString)
			#time.sleep(1) #delete maybe
	print("sendIMU thread done")

#-----------------MAIN----------------@
if __name__ == '__main__':
	pbstring = pbInit()
	serverAddress = ("128.39.200.239",20002)
	#serverAddress = ("b12.uia.no",20002)
	droneSocket = ClientSocket(serverAddress)
	connError = droneSocket.dserverConnect()
	print(connError)
	if not connError == 0:
		exit()

	#checklist before starting program
	checkList = True
	while(checkList):
		if droneSocket.globalTimer == None:
			print("timer missing, getting")
			dataMsg.Clear()
			dataMsg.ID = 2
			dataMsg.type = 0 #timeSync, MAKE ENUM!!!!!!!!!!!!!!!
			dataMsg.msg = "syncReq"
			droneSocket.send(dataMsg.SerializeToString())
			droneSocket.sSyncTimer(dataMsg)
			continue
		else:
			print("checkList completed")
			checkList = False
		

	qMotor = mp.Queue(5)

	tMotorRecv = tr.Thread(target=motorListen, args=(droneSocket,qMotor))
	tIMUsend = tr.Thread(target = sendIMU, args=(droneSocket,))
	tMotorRecv.run()
	tIMUsend.run()

	droneSocket.imuSend = 1
	time.sleep(1)
	droneSocket.motorRead = 1
	time.sleep(1)
	droneSOcket.imuSend = 2
	droneSocket.motorRead = 2

# print("sleeping")
# i = 0
#	while(i<=30):
#		print(qMotor.get())
#		time.sleep(1)
#		i += 1
#	print("joining threads")
	tMotorRecv.join()
	tIMUsend.join()

	if False:	 #block comment
		print("echoing")
		droneSocket.sock.settimeout(10)
		while(True):
			msg = droneSocket.recv()
			print("recieved: " + str(msg))
			droneSocket.send(str.encode("Recv: " + str(msg)))
