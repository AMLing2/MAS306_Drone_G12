# --------------------------------------- Libraries ---------------------------------------
import cv2
import cv2.aruco as aruco # For simplification
import pyrealsense2 as rs
import numpy
import csv
# --------------------------------------- Libraries ---------------------------------------

# ------------------- Constant variables for simple changes -------------------
axesLength = 0.1
screenHeight = 480   # pixels
screenWidth = 848    # pixels
fps = 60             # pixels
cColor = (0, 0, 120) # bgr
cThick = 1           # pixels

# Subpixel detection variables
iterStop = 30
cornerTolerance = 0.001
criteria = ( (cv2.TERM_CRITERIA_EPS + cv2.TermCriteria_MAX_ITER), iterStop, cornerTolerance)
winSize = (11, 11)      # OpenCV: "Size(5,5) , then a (5*2+1)x(5*2+1) = 11×11"
zeroZone = (-1, -1)     # Deadzone to avoid singularities. (-1,-1) = turned off

markerSize = 0.05 # Length of ArUco marker sides [m]
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

roundNr = 0
rotVectors = []
transVectors = []
reprojError = 0

with open('reprojErrors', 'w') as f:

    # Set writing variable for simplicity
    write = csv.writer(f)

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


                #cv2.cornerSubPix(gray, markerCorner, winSize, zeroZone, criteria) # Actually better w/o subpixel coords

                # Pose reading
                #rotVector, transVector, markerPoints = aruco.estimatePoseSingleMarkers(
                #    markerCorner, markerSize, cameraMatrix=cameraMatrix, distCoeffs=distortionCoefficients)
                #print("\nCornerMarker: ", markerCorner)


                # SolvePnPGeneric for extracting all possible vectors
                if (roundNr < 25):
                    retVal, rotVectors, transVectors, reprojError = cv2.solvePnPGeneric(
                        markerPoints, markerCorner, cameraMatrix, distortionCoefficients, rvecs=rotVectors, tvecs=transVectors, reprojectionError=reprojError)
                else:
                    print("\n------------ 25 has passed --------")
                    retVal, rotVectors, transVectors, reprojError = cv2.solvePnPGeneric(
                        markerPoints, markerCorner, cameraMatrix, distortionCoefficients, rvecs=rotVectors, tvecs=transVectors, reprojectionError=reprojError,
                        useExtrinsicGuess=False, flags=cv2.SOLVEPNP_IPPE, rvec=rotVector, tvec=transVector)

                print("\nRotation Vectors: ", rotVectors)
                print("\nTranslation Vectors: ", transVectors)

                # Extract guess
                rotVector = rotVectors[0]
                transVector = transVectors[0]

                #print("\nRotation Vectors length: ", len(rotVectors))
                print("\nReprojection Error: ", reprojError)
                write.writerow(reprojError)

                # Draw marker axes
                cv2.drawFrameAxes(color_image, cameraMatrix=cameraMatrix,
                                distCoeffs=distortionCoefficients, rvec=rotVector, tvec=transVector, length=axesLength)
                
                # Round and display translation vector
                transVector = numpy.around(transVector, 4)
                print("\nTranslation Vector: ", transVector)
                print("\nRotation Vector: ", rotVector) # Rotation vector

                # Draw marker axes
                cv2.drawFrameAxes(color_image, cameraMatrix=cameraMatrix,
                                distCoeffs=distortionCoefficients, rvec=rotVector, tvec=transVector, length=axesLength)
                
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

                # Display crosshair on Depth Stream
                    # Horizontal
                cv2.line(depth_image, (0, avgCorner_y), (screenWidth, avgCorner_y), cColor, cThick)
                    # Vertical
                cv2.line(depth_image, (avgCorner_x, 0), (avgCorner_x, screenHeight,), cColor, cThick)

                # Print Depth
                print("Depth Distance: ", depthDist)

                prevRotVector = rotVector

        # Depth Stream: Add color map
        depth_image = cv2.applyColorMap(cv2.convertScaleAbs(depth_image, alpha=0.15), cv2.COLORMAP_TURBO)

        # Display image
        cv2.imshow('DepthStream', depth_image)
        cv2.imshow('ColorStream', color_image)

        roundNr = roundNr+1

        # Loop breaker
        pressedKey = cv2.waitKey(1)
        if pressedKey == ord('q'):      # Press Q to stop
            break
        elif pressedKey == ord('p'):    # Press P to pause
            cv2.waitKey(-1)

pipe.stop()             # Stop recording
cv2.destroyAllWindows() # Free resources 