import pyrealsense2 as rs   # stream configuration
import cv2                  # Show video with OpenCV
import cv2.aruco as aruco   # Simplification
import os                   # Check file-extension
import numpy                # Python Math
import csv                  # Export to "Comma-Separated Values" file
import time                 # Computation Timer

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

# Directory to read/write videos
dir = r'/home/thomaz/Recordings'
os.chdir(dir)

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

# Create pipeline and config object
pipe = rs.pipeline()
cfg = rs.config()

# Size of window to display recording
displaySize = (960, 540)

# Store relevant values
framesWithData  = []    # Array to store frames with data
falsePositives  = []    # Array to store false positives
computationTime = []    # Array for computation times

# False positives detection tolerance - Change with empirical testing
falseTol = 0.085

# Export data to csv file
    # Path and name of file
csvFile = r'/home/thomaz/MAS306_Drone_G12/Code/Camera/Results/markerTestResults.csv'
    # (fileName, write/read, newline symbol set to nothing)
with open(csvFile, 'w', newline='') as file:
    
    # Create writer object
    csvwriter = csv.writer(file)

    # Headers
    csvwriter.writerow(['Dictionary', 'Rotation Vector', 'Translation Vector'])

    for dict in dictList:

        # Restart variables for relevant values
        startTimer = time.time() # Start timer per dict
        curDataFrames = 0        # Frames with data for current dict
        curFalsePos = 0          # False positives for current dict
        prevRotVec = [0, 0, 0]   # Restart rotation vector
        prevTransVec = [0, 0, 0] # Restart translation vector
        totalFrames = 0          # Total Frames extraction part 1

        # Fetch current dictionary
        dictionary = aruco.getPredefinedDictionary(dict) # <-- Tip from chatGPT, Detector_get is old

        # Import recording
        recording = cv2.VideoCapture('recording.avi')

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
                    rotVector, transVector, markerPoints = cv2.aruco.estimatePoseSingleMarkers(
                        corners[i], markerSize, cameraMatrix=cameraMatrix, distCoeffs=distortionCoefficients)

                    # Check for false positives - change in magnitude (Vector Norm |x|)
                    if ( ( (numpy.linalg.norm(prevTransVec) - numpy.linalg.norm(transVector)) > falseTol) ):
                        curFalsePos += 1

                    prevRotVec = rotVector
                    prevTransVec = transVector
                
                # Number of frames with data
                curDataFrames += 1

            # Write to CSV file
            csvwriter.writerow([dict, rotVector, transVector])
        
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

        # False Positives
        falsePositives.append(curFalsePos)

        
# Display relevant array values
print("\nComputation Time:\n", computationTime)
print("\nFrames With Data:\n", framesWithData)
print("\nFalse Positives.\n", falsePositives)
print("\nTotal Frames: ", totalFrames)