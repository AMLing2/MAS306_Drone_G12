# --------------------------------------- Libraries ---------------------------------------
import cv2
import cv2.aruco as aruco # For simplification
import pyrealsense2 as rs
import numpy
from numpy import sin, cos
from math import pi
# --------------------------------------- Libraries ---------------------------------------

# ------------------- Constant variables for simple changes -------------------
axesLength = 0.1
screenHeight = 480   # pixels
screenWidth = 848    # pixels
fps = 60             # pixels
cColor = (0, 0, 120) # bgr
cThick = 1           # pixels

markerSize = 0.05 # Length of ArUco marker sides [m]

# vvv INTRINSICS ARE FOR 848x480 vvv
    # Distortion Coefficients
distortionCoefficients = numpy.array(
    [ 0.0, 0.0, 0.0, 0.0, 0.0]) # [k1, k2, p1, p2, k3]

    # Intrinsic Camera Matrix
cameraMatrix = numpy.array([
[608.76301751,   0.0,         429.37397121],
[  0.0,         609.23981796, 232.71315263],
[  0.0,           0.0,          1.0        ]])

print("\nCamera Matrix\n", cameraMatrix)
print("\nDistortion Coefficients\n", distortionCoefficients)
 
# Angles [radians]
a = pi/2
b = pi
c = 0

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

yawPitchRoll = yaw @ pitch @ roll

yawPitchRoll = numpy.array([
    [-1, 0, 0],
    [0, 1, 0],
    [0, 0, -1]
])

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
pipe.start(config)

while(True):
    
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

            # Pose reading
            rotVector, transVector, markerPoints = aruco.estimatePoseSingleMarkers(
                markerCorner, markerSize, cameraMatrix=cameraMatrix, distCoeffs=distortionCoefficients)
            
            # Extract Rotation Matrix
            rMat, _ = cv2.Rodrigues(rotVector)
            print("Rotation Matrix: ", rMat)
            
            # Rotate marker display
            axes = rMat @ yawPitchRoll
            axes, _ = cv2.Rodrigues(axes)

            print("axes: ", axes)
            print("rotVector: ", rotVector)

            # Draw marker axes
            #cv2.drawFrameAxes(color_image, cameraMatrix=cameraMatrix,
            #                distCoeffs=distortionCoefficients, rvec=rotVector, tvec=transVector, length=axesLength)
            
            # Draw rotated axes
            cv2.drawFrameAxes(color_image, cameraMatrix=cameraMatrix,
                            distCoeffs=distortionCoefficients, rvec=axes, tvec=transVector, length=axesLength)
            
            # Round and display translation vector
            transVector = numpy.around(transVector, 4)
            print("\nTranslation Vector: ", transVector)
            
            # Extract corners
            corners = markerCorner.reshape(4,2)
            (topLeft, topRight, bottomRight, bottomLeft) = corners
            
            # Print Current marker ID
            print("Current ID: ", markerID)
            
            # Extract middle of marker
            avgCorner_x = int((topLeft[0] + topRight[0] + bottomLeft[0] + bottomRight[0])/4)
            avgCorner_y = int((topLeft[1] + topRight[1] + bottomLeft[1] + bottomRight[1])/4)

            # Extract depth distance at middle of marker
            depthDist = depth_image.item(avgCorner_y, avgCorner_x)

            # Display crosshair on Color Stream
                # Horizontal
            cv2.line(color_image, (0, avgCorner_y), (screenWidth, avgCorner_y), cColor, cThick)
                # Vertical
            cv2.line(color_image, (avgCorner_x, 0), (avgCorner_x, screenHeight,), cColor, cThick)

            # Print Depth
            print("Depth Distance: ", depthDist)



    # Depth Stream: Add color map
    # depth_image = cv2.applyColorMap(cv2.convertScaleAbs(depth_image, alpha=0.15), cv2.COLORMAP_TURBO)

    # Display image
    cv2.imshow('ColorStream', color_image)
    # cv2.imshow('DepthStream', depth_image)

    # Loop breaker
    pressedKey = cv2.waitKey(1)
    if pressedKey == ord('q'):      # Press Q to stop
        break
    elif pressedKey == ord('p'):    # Press P to pause
        cv2.waitKey(-1)

pipe.stop()             # Stop recording
cv2.destroyAllWindows() # Free resources 