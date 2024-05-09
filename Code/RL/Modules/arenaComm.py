import time
import socket
import queue
import multiprocessing as mp
from . import dronePosVec_pb2
from enum import Flag, auto

class SendrecvType(Flag):
    GENERICSEND = auto()
    GENERICRECV = auto()
    #GENERICSEND = 0b0100
    NO_MP =       auto()

class DataSending:
    mpLoop = True
    globalTimer = None
    offset = 0
    buffersize = 1024
    sendinterval = 100000000
    serverAddress = None
    nextAddress = None
    processType = None
    dataMsg = dronePosVec_pb2.dataTransfers()
    dp = dronePosVec_pb2.dronePosition()

    def __init__(self,serverAddress,queueCamera,processType):
        self.serverAddress = serverAddress
        self.sock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
        self.q = queueCamera #depracated
        self.processType = processType
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
        self.dataMsg.ID = self.processType
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
        self.dataMsg.ID = self.processType
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
        self.dataMsg.ID = self.processType
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
        self.dataMsg.ID = self.processType
        self.dataMsg.type = dronePosVec_pb2.stateChange
        self.dataMsg.msg = "stateChangeReq"
        self.send(self.dataMsg.SerializeToString())

        self.sock.shutdown(socket.SHUT_RDWR)
        self.sock.close()

        self.sock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM) #shutdown and then generate new socket
        self.sock.connect(self.nextAddress)
        self.dataMsg.Clear()
        self.dataMsg.ID = self.processType
        self.dataMsg.msg = "hi"
        self.send(self.dataMsg.SerializeToString())
        print("sucessfully connected to new socket")
    
    def checklist(self): #TODO: error handling
        checkList = True
        while(checkList):
            if self.globalTimer == None:
                print("timer missing, getting")
                self.dataMsg.Clear()
                self.dataMsg.ID = self.processType
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

    def waitForStartSignal(self):
        print("waiting for signal to start program")
        self.sock.settimeout(120)
        self.dataMsg.Clear()
        try:
            self.dataMsg.ParseFromString(self.recv())
        except Exception:
            return -1 #end program
        print(self.dataMsg.msg) #temporary
        if(self.dataMsg.type == dronePosVec_pb2.start):
            if self.dataMsg.timeSync_ns != 0: #change sending time
                self.sendinterval = self.dataMsg.timeSync_ns
            return 0
        else:
            return -1


class arenaCommunication:
    qSend = None
    qRecv = None
    pSend = None
    pRecv = None
    deviceType = None
    c = None
    dp = None
    dc = None
    matrixSize = []
    serverIP = None

    def __init__(self,startflags,hostname,matrixSize,deviceType):
        self.matrixSize[:] = matrixSize
        self.deviceType = deviceType
        #init drone position message:
        self.dp = dronePosVec_pb2.dronePosition()
        self.dp.Clear()
        self.dp.deviceType = deviceType
        self.dp.posShape[:] = self.matrixSize
        self.dp.rotShape[:] = self.matrixSize
        self.dc = dronePosVec_pb2.droneControl()
        self.dc.Clear()

        self.serverIP = socket.gethostbyname(hostname)

        if startflags != 0: #no start flags for manual socket starting (NO_MP reccomended)
            self.genericStarts(startflags)


    def genericStarts(self,startflags):
        if (SendrecvType.NO_MP in startflags) and ((SendrecvType.GENERICSEND in startflags) or (SendrecvType.GENERICRECV in startflags)): #exit so that multiple sockets arent created
            print("invalid flags")
            exit(-1)

        if (SendrecvType.NO_MP in startflags): # start unthreaded socket for custom writing
            self.c = DataSending(self.serverIP,self.qRecv,self.deviceType)
            if self.c.dserverConnect() == 0:
                self.c.checklist()
            return None

        self.qSend = mp.Queue(1)
        self.qRecv = mp.Queue(2)

        if (SendrecvType.GENERICSEND in startflags) and (SendrecvType.GENERICRECV in startflags): #starts generic socket that sends and recvs
            self.p = mp.Process(target=self.multiprocessRecvSend_,args=((self.serverIP,20002),self.qSend,self.qRecv,))
            self.p.start()
            return None

        elif (SendrecvType.GENERICSEND in startflags): # start send multiprocess only (for camera usually)
            self.p = mp.Process(target=self.multiprocessSend_,args=((self.serverIP,20002),self.qSend,))
            self.p.start()
        elif (SendrecvType.GENERICRECV in startflags): # start recv multiprocess only
            self.p = mp.Process(target=self.multiprocessRecv_,args=((self.serverIP,20002),self.qSend,))
            self.p.start()

    def addPosToSendQueue(self):
            if self.qSend.full() == True:
                try:
                    self.qSend.get(timeout=0.01) #empty if previous is still not read
                except Exception: #will be called if both threads try to get() at the same time
                    pass
            self.qSend.put(self.dp.SerializeToString())

    def addPosToRecvQueue(self):
        if self.qRecv.full() == True:
            try:
                self.qRecv.get(timeout=0.01) #empty if previous is still not read
            except Exception: #will be called if both threads try to get() at the same time
                pass
        self.qRecv.put(self.dp.SerializeToString())

    def addMotToSendQueue(self):
        if self.qSend.full() == True:
            try:
                self.qSend.get(timeout=0.01) #empty if previous is still not read
            except Exception: #will be called if both threads try to get() at the same time
                print(str(Exception))

        self.qSend.put(self.dc.SerializeToString())

    def endmps(self): #must be called before ending program if using multiprocesses
        self.p.join()

    # ----------------------------------- GENERIC RECV AND SENDS ----------------------------------
    # used for inbuilt functions such as camera or if not using self made recieve or sending methods for RL
    #generic sender (used for camera)
    def multiprocessSend_(self,serverAddress,queueCam):
        print("sender multiprocess starting")
        self.c = DataSending(serverAddress,queueCam,self.deviceType)

        timeout = None
        if self.c.dserverConnect() == 0:
            self.c.checklist()
            if self.c.waitForStartSignal() == -1:
                self.c.mpLoop = False #dont start program

            #main loop
            while(self.c.mpLoop):
                time.sleep(self.c.sleepTimeCalc(self.c.sendinterval,self.c.globalTimer))
                try:
                    #c.send(msg) #test
                    self.c.send(queueCam.get(block= True, timeout=timeout)) #real
                    timeout = 5 # set to 5s after initial
                except Exception as e:
                    self.c.mpLoop = False #timeout 
                    print("exception:" + repr(e))

        print("sender multiprocess closing")

    #generic reciever
    def multiprocessRecv_(self,serverAddress,queue):
        print("reciever multiprocess starting")
        self.c = DataSending(serverAddress,queue,self.deviceType)

        recvMsg = 0
        if self.c.dserverConnect() == 0:
            self.c.checklist()
            if self.c.waitForStartSignal() == -1:
                self.c.mpLoop = False #dont start program

            #main loop
            self.sock.settimeout(10.0) #long initial timeout for if server is still doing things
            while(self.c.mpLoop):
                time.sleep(self.c.sleepTimeCalc(self.c.sendinterval,self.c.globalTimer))
                try:
                    recvMsg = self.c.recv()
                    if queue.full() == True:
                        try:
                            queue.get(timeout=0.01) #empty if previous is still not read
                        except Exception: #will be called if both threads try to get() at the same time
                            pass
                    queue.put(recvMsg)

                    self.sock.settimeout(5.0)
                except Exception as e:
                    self.c.mpLoop = False #timeout 
                    print("exception:" + repr(e))

        print("reciever multiprocess closing")

     #generic synchronised send and reciever
    def multiprocessRecvSend_(self,serverAddress,qRecv,qSend):
        print("send and recieve multiprocess starting")
        self.c = DataSending(serverAddress,qRecv,self.deviceType)

        timeout = 10
        recvMsg = 0
        if self.c.dserverConnect() == 0:
            self.c.checklist()
            if self.c.waitForStartSignal() == -1:
                self.c.mpLoop = False #dont start program

            #main loop
            self.c.sock.settimeout(10.0) #long initial timeout for if server is still doing things
            while(self.c.mpLoop):
                time.sleep(self.c.sleepTimeCalc(self.c.sendinterval,self.c.globalTimer))
                try:
                    self.c.send(qSend.get(block= True, timeout=timeout)) #TODO: fix hangup
                    recvMsg = self.c.recv()
                    if self.qRecv.full() == True:
                        try:
                            self.qRecv.get(timeout=0.01) #empty if previous is still not read
                        except Exception: #will be called if both threads try to get() at the same time
                            pass
                    self.qRecv.put(recvMsg)
                    self.sock.settimeout(5.0)
                    timeout = 5 # set to 5s after initial
                except Exception as e:
                    self.c.mpLoop = False #timeout 
                    print("exception:" + repr(e))

        print("send and reciever multiprocess closing")