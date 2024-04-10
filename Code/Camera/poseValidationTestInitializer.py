# --------------------------------------- Libraries ---------------------------------------
import cv2
import cv2.aruco as aruco # For simplification
import pyrealsense2 as rs
import numpy
from numpy import sin, cos
import time
from math import pi
# --------------------------------------- Libraries ---------------------------------------

# ------------------- Constant variables for simple changes -------------------
axesLength = 0.1
screenHeight = 480   # pixels
screenWidth = 848    # pixels
fps = 60             # pixels
tolerance = 5 # degrees

# Angles [radians]
a = pi/2
b = pi/8
c = pi/8

# Rotation Matrix
yaw = numpy.array([
    [cos(a), -sin(a), 0],
    [sin(a),  cos(a), 0],
    [0,         0,    1]
])

pitch = numpy.array([
    [cos(b), 0, sin(b)],
    [0, 1, 0],
    [-sin(b), 0, cos(b)]
])

roll = numpy.array([
    [1, 0, 0],
    [0, cos(c), -sin(c)],
    [0, sin(c), cos(c)]
])
rotMatUpsideDown = yaw
rotMatUpsideDown = numpy.array([
    [-1.0, 0.0, 0.0],
    [ 0.0, 1.0, 0.0],
    [ 0.0, 0.0,-1.0]
])
rotVecUpsideDown, _ = cv2.Rodrigues(rotMatUpsideDown)


# Physical marker sizes
markerSize = 0.167 # Marker sides [m]
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
            print("\ntransVectors[0]: ", transVectors[0].flatten())

            # Draw marker axes
            cv2.drawFrameAxes(color_image, cameraMatrix=cameraMatrix,
                            distCoeffs=distortionCoefficients, rvec=rotVectors[0], tvec=transVectors[0], length=axesLength)
            
            # Print Current marker ID
            print("\nCurrent ID: ", markerID)
        
            # 
            curRotMat, _ = cv2.Rodrigues(rotVectors[0])
            print("\nCurrent Rotmat: ", curRotMat)
            rotMatDiff = numpy.dot(curRotMat, rotMatUpsideDown.T)
            rotVecDiff, _ = cv2.Rodrigues(rotMatDiff)
            diffAngle = numpy.rad2deg(numpy.linalg.norm(rotVecDiff))

            print("\nDiffAngle: ", diffAngle)

            if (diffAngle < tolerance):
                print("\nStop rotating, inside tolerance.")

    # Display image
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