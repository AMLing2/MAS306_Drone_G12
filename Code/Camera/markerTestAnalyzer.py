import cv2                  # Show video with OpenCV
import cv2.aruco as aruco   # Simplification
import os                   # Check file-extension
import numpy                # Python Math
import time                 # Computation Timer

font = cv2.FONT_HERSHEY_SIMPLEX
fontColor = (200, 20, 200)
fontScale = 0.5
fontThickness = 1
axesLength = 0.01

# Length of ArUco marker sides
markerSize = 0.05 # [m]

# Imported from Camera Calibration
distortionCoefficients = numpy.array(
    [ 0.0, 0.0, 0.0, 0.0, 0.0]) # [k1, k2, p1, p2, k3]
cameraMatrix = numpy.array([
[608.76301751,   0.0,         429.37397121],
[  0.0,         609.23981796, 232.71315263],
[  0.0,           0.0,          1.0        ]])

# Directory to read/write videos
dir = r'/home/thomaz/Recordings'
os.chdir(dir)

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

# Store relevant values
framesWithData  = []    # Array to store frames with data
computationTime = []    # Array for computation times


for dict in dictList:

    # Restart variables for relevant values
    startTimer = time.time() # Start timer per dict
    curDataFrames = 0        # Frames with data for current dict
    totalFrames = 0          # Total Frames extraction part 1

    # Fetch current dictionary
    dictionary = aruco.getPredefinedDictionary(dict)

    # Import recording for each dictionary
    recording = cv2.VideoCapture('recordingPostDepthTest.avi')

    while(recording.isOpened()):

        # Extract frames
        ret, frame = recording.read()
        
        # Total Frames extraction part 2
        totalFrames += 1

        # Stop while loop if no more frames
        if not ret:
            break

        # Identification
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)   # Grayscale image
        corners, ids, rejectedImagePoints = aruco.detectMarkers(gray, dictionary, parameters=arucoParams)
        
        if len(corners) > 0:
            for i in range(0, len(ids)):
                
                # Pose Estimate using ArUco
                rotVector, transVector, markerPoints = aruco.estimatePoseSingleMarkers(
                    corners[i], markerSize, cameraMatrix=cameraMatrix, distCoeffs=distortionCoefficients)

            # Number of frames with data
            curDataFrames += 1
    
        # Press Q to stop video playback
        if cv2.waitKey(1) == ord('q'):
            break
    
    # Release playback after each dictionary
    recording.release()

    # Computation Time
    endTimer = time.time()              # Time of loop stop
    currentTime = endTimer - startTimer # Time difference
    computationTime.append(currentTime) # Increment array

    # Number of Frames with Data
    framesWithData.append(curDataFrames)

        
# Display relevant array values
print("\nComputation Time:\n", computationTime)
print("\nFrames With Data:\n", framesWithData)
print("\nTotal Frames: ", totalFrames)