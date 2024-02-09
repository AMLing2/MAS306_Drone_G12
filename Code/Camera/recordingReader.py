
# --------------------------------------- Libraries ---------------------------------------
import pyrealsense2 as rs   # stream configuration
import cv2                  # Show video with OpenCV
import cv2.aruco as aruco   # Simplification
import argparse             # Argument Parsing
import os.path              # Check file-extension
import numpy
# --------------------------------------- Libraries ---------------------------------------

# ------------------- From arucoPoseEstimation -------------------
#markerColor = (255, 255, 0) # Corner color is set to be contrast to border color

font = cv2.FONT_HERSHEY_SIMPLEX
fontColor = (200, 20, 200)
fontScale = 0.5
fontThickness = 1
axesLength = 0.01

markerSize = 0.04 # Length of ArUco marker sides [m]

# Imported from Camera Calibration, hardcoded for testing first
cameraMatrix = numpy.array([
    [1379.333496,   0,         973.144470],     # [f_x, 0.0, c_x] used principal points 
    [  0,         1378.661255,  535.200500],    # [0.0, f_y, c_y] as optical center points
    [  0,           0,           1.0      ] ])  # [0.0, 0.0, 1.0]
distortionCoefficients = numpy.array(
    [ 0.0, 0.0, 0.0, 0.0, 0.0]) # [k1, k2, p1, p2, k3]

print("\nCamera Matrix\n", cameraMatrix)
print("\nDistortion Coefficients\n", distortionCoefficients)
# ------------------- Constant variables for simple changes -------------------

# ------------------------- ArUco Stuff -------------------------

# Set dictionary for the markers
arucoParams     = aruco.DetectorParameters()

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

# ------------------------- ArUco Stuff -------------------------

# Create pipeline and config object
pipe = rs.pipeline()
cfg = rs.config()

# Size of window to display recording
displaySize = (960, 540)

# File Path
bag_file_path = r'/home/thomaz/Recordings/recordedVideo.bag'

# Enable playback from the specified bag file
cfg.enable_device_from_file(bag_file_path)

# Start pipeline with configuration
pipe.start(cfg)

while(True):
    
    frame = pipe.wait_for_frames() # waits for and collects all frames from camera (depth, color, etc)
    color_frame = frame.get_color_frame()
    color_image = numpy.asanyarray(color_frame.get_data())

    # ------------------------------------ arucoPoseEstimation.py ------------------------------------

    for dict in dictList:
        arucoDictionary = aruco.getPredefinedDictionary(dict) # <-- Tip from chatGPT, Detector_get is old
        # Identification
        gray = cv2.cvtColor(color_image, cv2.COLOR_BGR2GRAY)   # Grayscale image
        corners, ids, rejectedImagePoints = aruco.detectMarkers(gray, arucoDictionary, parameters=arucoParams)

        
        if len(corners) > 0:
            for i in range(0, len(ids)):
                
                rotVector, transVector, markerPoints = cv2.aruco.estimatePoseSingleMarkers(
                    corners[i], markerSize, cameraMatrix=cameraMatrix, distCoeffs=distortionCoefficients)
                
                cv2.drawFrameAxes(color_image, cameraMatrix=cameraMatrix,
                                distCoeffs=distortionCoefficients, rvec=rotVector, tvec=transVector, length=axesLength)
    
    # ------------------------------------ arucoPoseEstimation.py ------------------------------------


    # Display Video
    displayWindow = cv2.resize(color_image, displaySize)    # Resize window
    cv2.imshow('LiveReading', displayWindow)                # Display the current frame
    
    # Press Q to stop video playback
    if cv2.waitKey(1) == ord('q'):
        break

pipe.stop()             # Stop recording
cv2.destroyAllWindows() # Free resources 

