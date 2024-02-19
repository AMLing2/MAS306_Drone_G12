import pyrealsense2 as rs   # stream configuration
import cv2                  # Show video with OpenCV
import cv2.aruco as aruco   # Simplification
import os                   # Check file-extension
import numpy                # Python Math
import csv                  # Export to "Comma-Separated Values" file

font = cv2.FONT_HERSHEY_SIMPLEX
fontColor = (200, 20, 200)
fontScale = 0.5
fontThickness = 1
axesLength = 0.01

# Length of ArUco marker sides
markerSize = 0.04 # [m]

# Imported from Camera Calibration, hardcoded for testing first
cameraMatrix = numpy.array([
    [1379.333496,   0,         973.144470],     # [f_x, 0.0, c_x] used principal points 
    [  0,         1378.661255,  535.200500],    # [0.0, f_y, c_y] as optical center points
    [  0,           0,           1.0      ] ])  # [0.0, 0.0, 1.0]
distortionCoefficients = numpy.array(
    [ 0.0, 0.0, 0.0, 0.0, 0.0]) # [k1, k2, p1, p2, k3]

# ------------------ Import/Export Video Setup ------------------
# Directory to save video 
dir = r'/home/thomaz/Recordings'
os.chdir(dir)

# Four-Character Code (File format)
fourcc = cv2.VideoWriter_fourcc(*'MJPG')

# Image Transfer Rate
fps = 60

# Image size [pixels]
w = 848 # Width
h = 480 # Height
# ------------------ Import/Export Video Setup ------------------

# Create object for parameters
arucoParams = aruco.DetectorParameters()

# Set dictionary for the markers
dictList = numpy.array([
	aruco.DICT_4X4_50,
	aruco.DICT_5X5_50,
	aruco.DICT_6X6_50,
	aruco.DICT_7X7_50,
	aruco.DICT_ARUCO_ORIGINAL,
	aruco.DICT_APRILTAG_16h5,
	aruco.DICT_APRILTAG_25h9,
	aruco.DICT_APRILTAG_36h10,
	aruco.DICT_APRILTAG_36h11,
    aruco.DICT_ARUCO_MIP_36h12
])

# Video number for filename
vidNr = 0

# Iterate through dict list
for dict in dictList:

    # Start new recording
    vidNr += 1
    resultVid = cv2.VideoWriter(f'results_{vidNr}.avi', fourcc, fps, (w,h))

    # Fetch current dictionary
    dictionary = aruco.getPredefinedDictionary(dict) # <-- Tip from chatGPT, Detector_get is old

    # Import recording
    recording = cv2.VideoCapture('recording.avi')

    while(recording.isOpened()):
        
        # Extract frames
        ret, frame = recording.read()        
        
        # Stop while loop if no more frames
        if not ret:
            break

        # Identification
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)   # Grayscale image
        corners, ids, rejectedImagePoints = aruco.detectMarkers(gray, dictionary, parameters=arucoParams)
        
        if len(corners) > 0:
            for i in range(0, len(ids)):
                
                # Pose Estimate using ArUco
                rotVector, transVector, markerPoints = cv2.aruco.estimatePoseSingleMarkers(
                    corners[i], markerSize, cameraMatrix=cameraMatrix, distCoeffs=distortionCoefficients)

                # Draw axes
                cv2.drawFrameAxes(frame, cameraMatrix=cameraMatrix,
                                distCoeffs=distortionCoefficients, rvec=rotVector, tvec=transVector, length=axesLength)

        # Display the current frame
        cv2.imshow('Playback', frame)  # Display the current frame
                
        # Export results video
        resultVid.write(frame)  # Incrementing filename

        # Press Q to stop video playback
        if cv2.waitKey(1) == ord('q'):
            break
    
    # Release playback after each dictionary
    recording.release()
    resultVid.release()