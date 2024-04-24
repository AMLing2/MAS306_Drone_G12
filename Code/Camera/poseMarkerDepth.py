# --------------------------------------- Libraries ---------------------------------------
import cv2
import cv2.aruco as aruco # For simplification
import pyrealsense2 as rs
import numpy
import time
import multiprocessing as mp
import socket
import dronePosVec_pb2 #should be protobuf.dronePosVec_pb2 but wont work for some reason

# --------------------------------------- Libraries ---------------------------------------
# --------------------------------------- Socket Class -----------------------------------
class DataSending:
    globalTimer = None
    offset = 0
    buffersize = 1024
    sendinterval = 100000000
    serverAddress = ("128.39.200.239",20002)
    dataMsg = dronePosVec_pb2.dataTransfers()
    dp = dronePosVec_pb2.dronePosition()

    def __init__(self,serverAddress):
        self.serverAddress = serverAddress
        self.sock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)

    def send(self,msg):
        self.sock.send(msg)

    def sSyncTimer(self): #sync local timer from server, probably outdated...
        self.dataMsg.Clear()
        self.sock.settimeout(None)
        msg = self.recv()
        self.globalTimer = time.time_ns()
        self.dataMsg.ParseFromString(msg)
        print("globalTImer: " +str(self.dataMsg.timeSync_ns))
        syncInterval = 100000000 #100ms
        sleepLen = self.sleepTimeCalc(syncInterval,self.globalTimer)
        time.sleep(sleepLen) #sleep until sync time
        responseTime = time.time_ns()
        print("responseTime: " + str(responseTime))

        self.dataMsg.Clear()
        self.dataMsg.ID = dronePosVec_pb2.drone #for drone probably
        self.dataMsg.timeSync_ns = responseTime
        self.dataMsg.msg = "responseTime" #probably ignored
        self.send(self.dataMsg.SerializeToString())

        self.dataMsg.ParseFromString(self.recv())
        self.offset = self.dataMsg.timeSync_ns
        self.globalTimer = self.globalTimer + self.offset
        print("offset: " + str(self.offset))

    def sleepTimeCalc(interval,timer):
        return float(interval - ((time.time_ns() - timer) % interval))/1000000000.0
    
    def send(self,msg):
        self.sock.send(msg)

    def dserverConnect(self): #needs to be updated, and pbstring changed 
        stringRecv = None
        pbstring = "hi"
        for i in range(10):
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
            return 1 #failure to send
        print("connecting to:" + str(stringRecv[1]))
        self.connect(stringRecv[1])
        self.send(pbstring) #connection msg
        return 0 #sucess
    
    def checklist(self):
        checkList = True
        while(checkList):
            if self.sock.globalTimer == None:
                print("timer missing, getting")
                self.dataMsg.Clear()
                self.dataMsg.ID = 2
                self.dataMsg.type = 0 #timeSync, MAKE ENUM!!!!!!!!!!!!!!!
                self.dataMsg.msg = "syncReq"
                self.send(self.dataMsg.SerializeToString())
                self.sSyncTimer(self.dataMsg)
                continue
            else:
                print("checkList completed")
                checkList = False



# --------------------------------------- Socket Class -----------------------------------
# ------------------- Constant variables for simple changes -------------------
axesLength = 0.1
screenHeight = 480   # pixels
screenWidth = 848    # pixels
fps = 60             # pixels
cColor = (0, 0, 120) # bgr
cThick = 1           # pixels

# Physical marker sizes
markerSize = 0.05 # Marker sides [m]
markerPoints = numpy.array([[-markerSize / 2, markerSize / 2, 0],
                              [markerSize / 2, markerSize / 2, 0],
                              [markerSize / 2, -markerSize / 2, 0],
                              [-markerSize / 2, -markerSize / 2, 0]], dtype=numpy.float32)

defaultRightCorner = numpy.array([((markerSize/2)*(1+1-1)/3)*1000, 
                                  ((markerSize/2)*(1+1-1)/3)*1000, 0.0])

# vvv INTRINSICS ARE FOR 848x480 vvv
    # Distortion Coefficients
distortionCoefficients = numpy.array(
    [ 0.0, 0.0, 0.0, 0.0, 0.0]) # [k1, k2, p1, p2, k3]

    # Intrinsic Camera Matrix
fx = 608.76301751
fy = 609.23981796
cx = 429.37397121
cy = 232.71315263
cameraMatrix = numpy.array([
[ fx,   0.0, cx ],
[ 0.0,  fy,  cy ],
[ 0.0,  0.0, 1.0]])

print("\nCamera Matrix\n", cameraMatrix)
print("\nDistortion Coefficients\n", distortionCoefficients)

# ------------------- Constant variables for simple changes -------------------

# Set dictionary for the markers
arucoParams     = aruco.DetectorParameters()
arucoDictionary = aruco.getPredefinedDictionary(aruco.DICT_5X5_50)

# Setup configuration and start pipeline stream
pipe = rs.pipeline()
config = rs.config()

# Color Stream
config.enable_stream(rs.stream.color, screenWidth, screenHeight, rs.format.bgr8, fps) # (streamType, xRes, yRes, format, fps)
# Depth Stream
config.enable_stream(rs.stream.depth, screenWidth, screenHeight, rs.format.z16, fps) # (streamType, xRes, yRes, format, fps)
#config.enable_stream(rs.stream.infrared, screenWidth, screenHeight, rs.format.y8, fps) # (streamType, xRes, yRes, format, fps)
pipe.start(config)

rotVectors = []
transVectors = []
reprojError = 0

while(True):

    startTime = time.time_ns()
    
    # Frame Collection
    frame = pipe.wait_for_frames()          # collect frames from camera (depth, color, etc)
    
    # Color Stream
    color_frame = frame.get_color_frame()                  # Extract Color frame
    color_image = numpy.asanyarray(color_frame.get_data()) # Convert to NumPy array

    # Depth Stream
    depth_frame = frame.get_depth_frame()    # Extract Depth frame
    depth_image = numpy.asanyarray(depth_frame.get_data()) # Convert to NumPy array

    # Marker Identification
    gray = cv2.cvtColor(color_image, cv2.COLOR_BGR2GRAY)   # Grayscale image
    corners, ids, rejectedImagePoints = aruco.detectMarkers(gray, arucoDictionary, parameters=arucoParams)

    # Is marker detected?
    if len(corners) > 0:
        
        # Iterate through list of markers
        for (markerCorner, markerID) in zip(corners, ids):

            # SolvePnPGeneric for extracting all possible vectors
            retVal, rotVectors, transVectors, reprojError = cv2.solvePnPGeneric(
                markerPoints, markerCorner, cameraMatrix, distortionCoefficients, rvecs=rotVectors, tvecs=transVectors, reprojectionError=reprojError,
                useExtrinsicGuess=False, flags=cv2.SOLVEPNP_IPPE)

            # Print current vectors
            print("\nRotation Vectors: ", rotVectors)
            print("\nTranslation Vectors: ", transVectors)

            # Reprojection Error Logging
            print("\nReprojection Error: ", reprojError)
            #write.writerow(reprojError)

            # Draw marker axes
            cv2.drawFrameAxes(color_image, cameraMatrix=cameraMatrix,
                            distCoeffs=distortionCoefficients, rvec=rotVectors[0], tvec=transVectors[0], length=axesLength)

            # Draw marker axes
            cv2.drawFrameAxes(color_image, cameraMatrix=cameraMatrix,
                            distCoeffs=distortionCoefficients, rvec=rotVectors[0], tvec=transVectors[0], length=axesLength)
            
            # Print Current marker ID
            print("\nCurrent ID: ", markerID)

            # Extract corners
            corners = markerCorner.reshape(4,2)
            (topLeft, topRight, bottomRight, bottomLeft) = corners

            # Extract middle of marker
            avgCorner_x = int((topLeft[0] + topRight[0] + bottomLeft[0] + bottomRight[0])/4)
            avgCorner_y = int((topLeft[1] + topRight[1] + bottomLeft[1] + bottomRight[1])/4)

            # Extract depth distance at middle of marker
            depthDist = depth_image.item(avgCorner_y, avgCorner_x)

            # Print Depth
            print("Depth Distance: ", depthDist)

    # Depth Stream: Add color map
    depth_image = cv2.applyColorMap(cv2.convertScaleAbs(depth_image, alpha=0.15), cv2.COLORMAP_TURBO)

    # Display image
    cv2.imshow('DepthStream', depth_image)
    cv2.imshow('ColorStream', color_image)

    # Loop breaker
    pressedKey = cv2.waitKey(1)
    if pressedKey == ord('q'):      # Press Q to stop
        break
    elif pressedKey == ord('p'):    # Press P to pause
        cv2.waitKey(-1)

    endTime = time.time_ns()
    diffTime = endTime - startTime
    #print("\nThis frame [ns]: ", diffTime)
    #print("\nTimestamp [ns]: ", startTime)

    # Variabler for Adrian export:
        # startTime, rotVectors, transVectors[0], depthDist, markerID

pipe.stop()             # Stop recording
cv2.destroyAllWindows() # Free resources