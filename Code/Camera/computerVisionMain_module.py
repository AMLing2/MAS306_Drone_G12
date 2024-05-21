# --------------------------------------- Libraries ---------------------------------------
import cv2                      # OpenCV
import cv2.aruco as aruco       # For simplification
import pyrealsense2 as rs       # RealSense Wrapper
import numpy                    #
import time
from comm_module import dronePosVec_pb2 #should be protobuf.dronePosVec_pb2 but wont work for some reason
from comm_module import arenaComm

# --------------------------------------- Libraries ---------------------------------------
# --------------------------------------- Socket Class -----------------------------------

#START MULTIPROCESS
hostname = 'b12.uia.no'
messager = arenaComm.ArenaCommunication(arenaComm.SendrecvType.GENERICSEND,hostname,(1,3),(3,3),dronePosVec_pb2.camera)

# --------------------------------------- Socket Class -----------------------------------
# ------------------- Constant variables for simple changes -------------------
axesLength = 0.1
screenHeight = 480   # pixels
screenWidth = 848    # pixels
fps = 60             # pixels
#cColor = (0, 0, 120) # bgr
#cThick = 1           # pixels

# Physical marker sizes
markerSize = 0.05 # Marker sides [m]
markerPoints = numpy.array([[-markerSize / 2, markerSize / 2, 0],
                            [markerSize / 2, markerSize / 2, 0],
                            [markerSize / 2, -markerSize / 2, 0],
                            [-markerSize / 2, -markerSize / 2, 0]], dtype=numpy.float32)

#defaultRightCorner = numpy.array([((markerSize/2)*(1+1-1)/3)*1000, 
#                                ((markerSize/2)*(1+1-1)/3)*1000, 0.0])

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
#config.enable_stream(rs.stream.depth, screenWidth, screenHeight, rs.format.z16, fps) # (streamType, xRes, yRes, format, fps)
#config.enable_stream(rs.stream.infrared, screenWidth, screenHeight, rs.format.y8, fps) # (streamType, xRes, yRes, format, fps)
pipe.start(config)

rotVectors = []
transVectors = []
reprojError = 0
m  = 0      # iterator
rotMatExp1 = numpy.empty(messager.rotShape[0] * messager.rotShape[1],dtype=float) #exp = export
rotMatExp2 = numpy.empty(messager.rotShape[0] * messager.rotShape[1],dtype=float)
transVectorsExp = numpy.empty(messager.posShape[0] * messager.posShape[1],dtype=float)

while(True):
    
    # Frame Collection
    frame = pipe.wait_for_frames()          # collect frames from camera (depth, color, etc)
    messager.dp.timestamp_ns = time.time_ns() #timestamp of when the image is taken
    # Color Stream
    color_frame = frame.get_color_frame()                  # Extract Color frame
    color_image = numpy.asanyarray(color_frame.get_data()) # Convert to NumPy array

    # Depth Stream
    #depth_frame = frame.get_depth_frame()    # Extract Depth frame
    #depth_image = numpy.asanyarray(depth_frame.get_data()) # Convert to NumPy array

    # Marker Identification
    gray = cv2.cvtColor(color_image, cv2.COLOR_BGR2GRAY)   # Grayscale image
    corners, ids, rejectedImagePoints = aruco.detectMarkers(gray, arucoDictionary, parameters=arucoParams)

    ############ Export Depth and Color frames here ############
    #print("\nDepth Image: ", depth_image)

    # Is marker detected?
    if len(corners) > 0:
        
        # Iterate through list of markers
        for (markerCorner, markerID) in zip(corners, ids):

            # SolvePnPGeneric for extracting all possible vectors
            retVal, rotVectors, transVectors, reprojError = cv2.solvePnPGeneric(
                markerPoints, markerCorner, cameraMatrix, distortionCoefficients, rvecs=rotVectors, tvecs=transVectors, reprojectionError=reprojError,
                useExtrinsicGuess=False, flags=cv2.SOLVEPNP_IPPE)

            # Print current vectors
            #print("\nRotation Vectors: ", rotVectors)
            #print("\nTranslation Vectors: ", transVectors)

            # Extract Rotation Matrices
            rMat1, _ = cv2.Rodrigues(rotVectors[0])
            rMat2, _ = cv2.Rodrigues(rotVectors[1])
            #print("Rotation Matrix: ", rMat)

            # Reprojection Error Logging
            #print("\nReprojection Error: ", reprojError)
            #write.writerow(reprojError)

            # Draw marker axes
            cv2.drawFrameAxes(color_image, cameraMatrix=cameraMatrix,
                            distCoeffs=distortionCoefficients, rvec=rotVectors[0], tvec=transVectors[0], length=axesLength)

            # Extract Matrices (flatten)
            #flatten rotation:
            m = 0
            for i in range(messager.rotShape[0]):
                for n in range(messager.rotShape[1]):
                    #print("m: " + str(m) + " i: " +  str(i) + " n: " + str(n))
                    rotMatExp1[m] = rMat1[i][n]
                    rotMatExp2[m] = rMat2[i][n]
                    m += 1
            #flatten position:
            m = 0
            for i in range(messager.posShape[0]):
                for n in range(messager.posShape[1]):
                    transVectorsExp[m] = transVectors[i][n]
                    m += 1
            messager.dp.rotation[:] = rotMatExp1
            messager.dp.rotation2[:] = rotMatExp1
            messager.dp.position[:] = transVectorsExp

    # Depth Stream: Add color map
    #depth_image = cv2.applyColorMap(cv2.convertScaleAbs(depth_image, alpha=0.15), cv2.COLORMAP_TURBO)

    # Display image
    #cv2.imshow('DepthStream', depth_image)
    cv2.imshow('ColorStream', color_image)

    # Loop breaker
    pressedKey = cv2.waitKey(1)
    if pressedKey == ord('q'):      # Press Q to stop
        break
    elif pressedKey == ord('p'):    # Press P to pause
        cv2.waitKey(-1)


    messager.addPosToSendQueue()
    del messager.dp.rotation[:]
    del messager.dp.rotation2[:]
    del messager.dp.position[:]

pipe.stop()             # Stop recording
cv2.destroyAllWindows() # Free resources
messager.endmps()
