import time
import socket
import queue
import multiprocessing as mp
from . import dronePosVec_pb2
from enum import Flag, auto
import traceback #temp, for debugging
import csv

class SendrecvType(Flag):
    GENERICSEND = auto()
    GENERICRECV = auto()
    #GENERICSEND = 0b0100
    NO_MP =       auto()

__global_Server_Time = 0
__interval_Send = 100000000

def getTimeNow(): #returns ns
    return time.time_ns()

#def intervalTimePrev(): #returns ns
#    return float(time.time_ns() - ((time.time_ns() - __global_Server_Time) % __interval_Send) - __interval_Send)

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

    def __init__(self,serverAddress,processType):
        self.serverAddress = serverAddress
        self.sock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
        self.processType = processType
        self.sock.setblocking(True)
        global __interval_Send
        __interval_Send = self.sendinterval

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
        global __global_Server_Time
        __global_Server_Time = self.globalTimer
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
                global __interval_Send
                __interval_Send = self.sendinterval #update the global interval
            return 0
        else:
            return -1


class ArenaCommunication:
    expectedTime = 0 #temp
    qSend = None
    qRecv = None
    mpEvent = mp.Event()
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
            self.c = DataSending(self.serverIP,self.deviceType)
            if self.c.dserverConnect() == 0:
                self.c.checklist()
            return None

        self.qSend = mp.Queue(2)
        self.qRecv = mp.Queue(2)

        if (SendrecvType.GENERICSEND in startflags) and (SendrecvType.GENERICRECV in startflags): #starts generic socket that sends and recvs
            self.p = mp.Process(target=self.multiprocessRecvSend_,args=((self.serverIP,20002),))
            self.p.start()
            return None

        elif (SendrecvType.GENERICSEND in startflags): # start send multiprocess only (for camera usually)
            self.p = mp.Process(target=self.multiprocessSend_,args=((self.serverIP,20002),))
            self.p.start()
        elif (SendrecvType.GENERICRECV in startflags): # start recv multiprocess only
            self.p = mp.Process(target=self.multiprocessRecv_,args=((self.serverIP,20002),))
            self.p.start()

    def safeQueueGet(self,_q,_block,_timeout):
        try:
            self.mpEvent.set()
            qval = _q.get(block= _block, timeout=_timeout) #class exception here
            self.mpEvent.clear()
            return qval
        except Exception as e:
            print("exception:" + repr(e))
            self.mpEvent.clear()
            return str(0)

    def addPosToSendQueue(self):
            self.qSend.put(self.dp.SerializeToString())
            if (self.qSend.full() == True) and (not (self.mpEvent.is_set())): #kind of bad use of a queue because 
                try:
                    self.qSend.get(timeout=0.01) #empty if previous is still not read
                except Exception as e: #will be called if both threads try to get() at the same time
                    print("add to queue: exception:" + repr(e))
            

    def addPosToRecvQueue(self):
        self.qRecv.put(self.dp.SerializeToString())
        if (self.qRecv.full() == True) and (not (self.mpEvent.is_set())):
            try:
                self.qRecv.get(timeout=0.05) #empty if previous is still not read
            except Exception as e: #will be called if both threads try to get() at the same time
                print("add to queue: exception:" + repr(e))
        

    def addMotToSendQueue(self):
        self.qSend.put(self.dc.SerializeToString(),True,None)
        if (self.qSend.full() == True) and (not (self.mpEvent.is_set())):
            try:
                self.qSend.get(timeout=0.01) #empty 1st in queue only, if previous is still not read
            except Exception as e: #will be called if both threads try to get() at the same time
                print("add to queue: exception:" + repr(e))

    def endmps(self): #must be called before ending program if using multiprocesses
        self.p.join()

    # ----------------------------------- GENERIC RECV AND SENDS ----------------------------------
    # used for inbuilt functions such as camera or if not using self made recieve or sending methods for RL
    #generic sender (used for camera)
    def multiprocessSend_(self,serverAddress):
        print("sender multiprocess starting")
        self.c = DataSending(serverAddress,self.deviceType)

        qtimeout = 10
        if self.c.dserverConnect() == 0:
            self.c.checklist()
            if self.c.waitForStartSignal() == -1:
                self.c.mpLoop = False #dont start program

            #main loop
            while(self.c.mpLoop):
                time.sleep(self.c.sleepTimeCalc(self.c.sendinterval,self.c.globalTimer))
                try:
                    #c.send(msg) #test
                    self.c.send(self.safeQueueGet(self.qSend,True,qtimeout))
                    qtimeout = 5 # set to 5s after initial
                except Exception as e:
                    self.c.mpLoop = False #timeout 
                    print("exception:" + repr(e))
                    break

        print("sender multiprocess closing")

    #generic reciever
    def multiprocessRecv_(self,serverAddress):
        print("reciever multiprocess starting")
        self.c = DataSending(serverAddress,self.deviceType)

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
                    if (self.qSend.full() == True) and (not (self.mpEvent.is_set())):
                        try:
                            self.qSend.get(timeout=0.05) #empty if previous is still not read
                        except Exception: #will be called if both threads try to get() at the same time
                            pass
                    self.qSend.put(recvMsg)

                    self.sock.settimeout(5.0)
                except Exception as e:
                    self.c.mpLoop = False #timeout 
                    print("exception:" + repr(e))
                    break

        print("reciever multiprocess closing")

     #generic synchronised send and reciever
    def multiprocessRecvSend_(self,serverAddress):
        print("send and recieve multiprocess starting")
        self.c = DataSending(serverAddress,self.deviceType)
        #csv
        fields = ['time diff']
        filename = "times.csv"
        #csv
        with open(filename, 'w') as csvfile:
            writer = csv.writer(csvfile)
            writer.writerow(fields)

            qtimeout = 10
            recvMsg = []
            if self.c.dserverConnect() == 0:
                self.c.checklist()
                if self.c.waitForStartSignal() == -1:
                    self.c.mpLoop = False #dont start program

                #main loop
                self.c.sock.settimeout(10.0) #long initial timeout for if server is still doing things
                while(self.c.mpLoop):
                    self.expectedTime = (self.c.sleepTimeCalc(self.c.sendinterval,self.c.globalTimer)*1000000000.0) + getTimeNow()

                    print(self.c.sleepTimeCalc(self.c.sendinterval,self.c.globalTimer))
                    time.sleep(self.c.sleepTimeCalc(self.c.sendinterval,self.c.globalTimer))
                    #self.expectedTime = getTimeNow()
                    try:
                        self.c.send(self.safeQueueGet(self.qSend,True,qtimeout))
                        recvMsg = self.c.recv()
                        if self.qRecv.full() == True:
                            try:
                                self.qRecv.get(timeout=0.05) #empty if previous is still not read
                            except Exception: #will be called if both threads try to get() at the same time
                                pass
                        self.qRecv.put(recvMsg)
                        writer.writerow({(getTimeNow() - self.expectedTime)/1000000})
                        print("msg recieved with time difference (ms): " + str((getTimeNow() - self.expectedTime)/1000000))
                        self.c.sock.settimeout(5.0)
                        qtimeout = 5 # set to 5s after 
                    except Exception as e:
                        self.c.mpLoop = False #timeout 
                        print("exception:" + repr(e))
                        break

        print("send and reciever multiprocess closing")