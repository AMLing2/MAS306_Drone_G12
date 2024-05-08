import time
import socket
import queue
import multiprocessing as mp
import dronePosVec_pb2

class DataSending:
    mpLoop = True
    globalTimer = None
    offset = 0
    buffersize = 1024
    sendinterval = 100000000
    serverAddress = None #("128.39.200.239",20002) #dont hardcode this!!!!!!!!!!!!!!!!!1
    nextAddress = None
    dataMsg = dronePosVec_pb2.dataTransfers()
    dp = dronePosVec_pb2.dronePosition()

    def __init__(self,serverAddress,queueCamera):
        self.serverAddress = serverAddress
        self.sock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
        self.q = queueCamera
        self.sock.setblocking(True)

    def send(self,msg):
        self.sock.send(msg)

    def recv(self):
        return self.sock.recv(self.buffersize)

    def sSyncTimer(self): #sync local timer from server, probably outdated...
        self.dataMsg.Clear()
        self.sock.settimeout(None)
        msg = self.recv()
        self.globalTimer = time.time_ns()
        self.dataMsg.ParseFromString(msg)
        syncInterval = 100000000 #100ms
        sleepLen = self.sleepTimeCalc(syncInterval,self.globalTimer)
        time.sleep(sleepLen) #sleep until sync time
        responseTime = time.time_ns()
        print("responseTime: " + str(responseTime))

        self.dataMsg.Clear()
        self.dataMsg.ID = dronePosVec_pb2.camera
        self.dataMsg.timeSync_ns = responseTime
        self.dataMsg.msg = "responseTime" #probably ignored
        self.send(self.dataMsg.SerializeToString())

        self.dataMsg.ParseFromString(self.recv())
        self.offset = self.dataMsg.timeSync_ns
        self.globalTimer = self.globalTimer - self.offset
        print("offset: " + str(self.offset))

    def sleepTimeCalc(self,interval,timer):
        return float(interval - ((time.time_ns() - timer) % interval))/1000000000.0
    
    def initRecv(self):
        return self.sock.recvfrom(self.buffersize)

    def dserverConnect(self): #needs to be updated, and pbstring changed 
        stringRecv = None
        self.dataMsg.Clear()
        self.dataMsg.ID = dronePosVec_pb2.camera
        self.dataMsg.msg = "hi"
        pbstring = self.dataMsg.SerializeToString()
        for i in range(10):
            #print("sendto: " + str(self.serverAddress))
            self.sock.sendto(pbstring,self.serverAddress) #initsend
            try:
                self.sock.settimeout(3.0)
                stringRecv = self.initRecv() #inital send
                print("string recieved: " + str(stringRecv))
            except:
                print("timeout, retrying " + str(i))
            if not stringRecv == None:
                break
        if stringRecv == None:
            return -1 #failure to send
        print("connecting to:" + str(stringRecv[1]))
        self.sock.connect(stringRecv[1])
        #self.send(pbstring) #connection msg
        return 0 #sucess
    
    def getnextAddr(self):
        print("next address missing, getting")
        self.dataMsg.Clear()
        self.dataMsg.ID = dronePosVec_pb2.camera
        self.dataMsg.type = dronePosVec_pb2.socketInfo
        self.dataMsg.msg = "socketReq"
        self.send(self.dataMsg.SerializeToString())

        self.sock.settimeout(None)
        stringRecv = self.recv()
        self.dataMsg.Clear()
        self.dataMsg.ParseFromString(stringRecv)
        print("next address: " + str(self.dataMsg.IP)+ ":" + str(self.dataMsg.port))
        return (self.dataMsg.IP,self.dataMsg.port)
    
    def stateChange(self):
        self.dataMsg.Clear()
        self.dataMsg.ID = dronePosVec_pb2.camera
        self.dataMsg.type = dronePosVec_pb2.stateChange
        self.dataMsg.msg = "stateChangeReq"
        self.send(self.dataMsg.SerializeToString())

        self.sock.shutdown(socket.SHUT_RDWR)
        self.sock.close()

        self.sock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM) #shutdown and then generate new socket
        self.sock.connect(self.nextAddress)
        self.dataMsg.Clear()
        self.dataMsg.ID = dronePosVec_pb2.camera
        self.dataMsg.msg = "hi"
        self.send(self.dataMsg.SerializeToString())
    
    def checklist(self):
        checkList = True
        while(checkList):
            if self.globalTimer == None:
                print("timer missing, getting")
                self.dataMsg.Clear()
                self.dataMsg.ID = dronePosVec_pb2.camera
                self.dataMsg.type = dronePosVec_pb2.timeSync
                self.dataMsg.msg = "syncReq"
                self.send(self.dataMsg.SerializeToString())
                self.sSyncTimer()
                continue
            elif self.nextAddress == None:
                self.nextAddress = self.getnextAddr()
            else:
                print("checkList completed, requesting state change")
                self.stateChange()
                checkList = False

class arenaSendPos: #for camera mostly, set usingGenericSend false if using own sending method
    q = None
    p = None
    c = None
    dp = None
    matrixSize = [2,3]
    def __init__(self,usingGenericSend):
        if usingGenericSend:
            self.q = mp.Queue(1)
            self.p = mp.Process(target=self.multiprocessFunc,args=(('128.39.200.239',20002),self.q,))
            self.p.start()
        #init drone position message:
        self.dp = dronePosVec_pb2.dronePosition()
        self.dp.Clear()
        self.dp.deviceType = dronePosVec_pb2.CameraOnly
        self.dp.matrixSize[:] = self.matrixSize

    def multiprocessFunc(self,serverAddress,queueCam):
        print("sender multiprocess starting")
        self.c = DataSending(serverAddress,queueCam)

        timeout = None
        if self.c.dserverConnect() == 0:
            self.c.checklist()
            while(self.c.mpLoop):
                time.sleep(self.c.sleepTimeCalc(self.c.sendinterval,self.c.globalTimer))
                try:
                    #c.send(msg) #test
                    self.c.send(self.q.get(block= True, timeout=timeout)) #real
                    timeout = 5 # set to 5s after initial
                except Exception as e:
                    self.c.mpLoop = False #timeout 
                    print("exception:" + repr(e))

        print("sender multiprocess closing")

    def addPosToQueue(self):
        if self.q.full() == True:
            try:
                self.q.get_nowait() #empty if previous is still not read
            except Exception: #will be called if both threads try to get() at the same time
                pass
        self.q.put(self.dp.SerializeToString())

    def endRecv(self): #must be called before ending program
        self.c.mpLoop = False
        self.p.join()

class arenaRecvPos: #set usingGenericRecv false if not using queue method
    q = None
    p = None
    c = None
    dp = None
    def __init__(self,usingGenericRecv):
        if usingGenericRecv:
            self.q = mp.Queue(1)
            self.p = mp.Process(target=self.multiprocessFunc,args=(('128.39.200.239',20002),self.q,))
            self.p.start()
        self.dp = dronePosVec_pb2.dronePosition()
        self.dp.Clear()
        self.dp.deviceType = dronePosVec_pb2.CameraOnly
        matrixSize = [2,3]
        self.dp.matrixSize[:] = matrixSize

    def multiprocessFunc(self,serverAddress,queueCam):
        print("sender multiprocess starting")
        self.c = DataSending(serverAddress,queueCam)

        timeout = None
        if self.c.dserverConnect() == 0:
            self.c.checklist()
            while(self.c.mpLoop):
                time.sleep(self.c.sleepTimeCalc(self.c.sendinterval,self.c.globalTimer))
                try:
                    self.c.recv()
                    timeout = 5 # set to 5s after initial
                except Exception as e:
                    self.c.mpLoop = False #timeout 
                    print("exception:" + repr(e))

        print("sender multiprocess closing")

    def addPosToQueue(self):
        if self.q.full() == True:
            try:
                self.q.get_nowait() #empty if previous is still not read
            except Exception: #will be called if both threads try to get() at the same time
                pass

        self.q.put(self.dp.SerializeToString())

    def endRecv(self): #must be called before ending program
        self.c.mpLoop = False
        self.p.join()

class droneSender:
    q = None
    p = None
    c = None
    dc = None
    def __init__(self,usingGenericSend):
        if usingGenericSend:
            self.q = mp.Queue(1)
            self.p = mp.Process(target=self.multiprocessFunc,args=(('128.39.200.239',20002),self.q,))
            self.p.start()
        self.dp = dronePosVec_pb2.droneControl()
        self.dp.Clear()

    def multiprocessFunc(self,serverAddress,queueCam):
        print("sender multiprocess starting")
        self.c = DataSending(serverAddress,queueCam)

        timeout = None
        if self.c.dserverConnect() == 0:
            self.c.checklist()
            while(self.c.mpLoop):
                time.sleep(self.c.sleepTimeCalc(self.c.sendinterval,self.c.globalTimer))
                try:
                    #c.send(msg) #test
                    self.c.send(self.q.get(block= True, timeout=timeout)) #real
                    timeout = 5 # set to 5s after initial
                except Exception as e:
                    self.c.mpLoop = False #timeout 
                    print("exception:" + repr(e))

        print("sender multiprocess closing")

    def addPosToQueue(self):
        if self.q.full() == True:
            try:
                self.q.get_nowait() #empty if previous is still not read
            except Exception: #will be called if both threads try to get() at the same time
                pass
        self.q.put(self.dp.SerializeToString())

    def endRecv(self): #must be called before ending program
        self.c.mpLoop = False
        self.p.join()