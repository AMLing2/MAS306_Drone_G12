# --------------------------------------- Libraries ---------------------------------------
import cv2                  # Show video with OpenCV
import cv2.aruco as aruco   # Simplification
import os                   # Check file-extension
import numpy                # Python Math
import csv                  # Data import
# --------------------------------------- Libraries ---------------------------------------

# ------------------- Constant variables for simple changes -------------------
axesLength = 0.1
screenHeight = 480   # pixels
screenWidth = 848    # pixels
fps = 60             # pixels

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

# ------------------ Import/Export Setup ------------------
# Directory to save video 
dir = r'/home/thomaz/poseValidationTest'
os.chdir(dir)

# Four-Character Code (File format)
fourcc = cv2.VideoWriter_fourcc(*'MJPG')

# Image Transfer Rate
fps = 60

# Image size [pixels]
w = 848 # Width
h = 480 # Height

# Test number for easy change
testNr = 4
# ------------------ Import/Export Setup ------------------

# Create object for parameters
arucoParams = aruco.DetectorParameters()

# Set dictionary for the markers
arucoParams     = aruco.DetectorParameters()
arucoDictionary = aruco.getPredefinedDictionary(aruco.DICT_5X5_50)

rotVectors = []
transVectors = []
reprojError = 0
loopRound = 0

# Start new recording
resultVid = cv2.VideoWriter(f'resultsPoseTest_{testNr}.avi', fourcc, fps, (w,h))

# Import recording
recording = cv2.VideoCapture(f'qualisysTest_{testNr}.avi')

# Import timestamps from D435 recording
fileOpenCV = open(f'cameraTimestamps_{testNr}.csv')
dataOpenCVreader = csv.reader(fileOpenCV)
dataOpenCV = []
for row in dataOpenCVreader:
    dataOpenCV.append(row)

timestampOpenCV = [row[1] for row in dataOpenCV[1:]]
timestampOpenCV = numpy.array(timestampOpenCV, dtype=float)
iterOpenCV = [row[0] for row in dataOpenCV[1:]]
iterOpenCV = numpy.array(iterOpenCV, dtype=int)

#print("\ndataOpenCV: ", dataOpenCV)
print("\nTimestampsOpenCV: ", timestampOpenCV)

# Setup csv file
filename = f"ExportedAruco_{testNr}.csv" 
fields = ['Frame', 'TimeCV', 'rVec0_0', 'rVec0_1', 'rVec0_2', 'rVec1_0', 'rVec1_1', 'rVec1_2', 'reprojError0', 'reprojError1']

# Column names
with open(filename, 'w') as csvfile:
		csvwriter = csv.writer(csvfile)
		csvwriter.writerow(fields)
          

with open(filename,'a') as csvfile:
    # Set writer for loop
    csvwriter = csv.writer(csvfile)
        
    while(recording.isOpened()):
        
        # Extract frames
        ret, frame = recording.read()        
        
        # Stop while loop if no more frames
        if not ret:
            break

        # Identification
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)   # Grayscale image
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
    #            print("\nRotation Vectors: ", rotVectors)
    #            print("\nTranslation Vectors: ", transVectors)

                # Draw marker axes
                cv2.drawFrameAxes(frame, cameraMatrix=cameraMatrix,
                                distCoeffs=distortionCoefficients, rvec=rotVectors[0], tvec=transVectors[0], length=axesLength)
                
                # Print Current marker ID
    #            print("\nCurrent ID: ", markerID)

#            print("\nRotVector: ", rotVectors[1])
#            print("\nRotVectorElement: ", rotVectors[1][1][0])

            #print("\nreprojError: ", reprojError)
            #print("\nreprojErrorElement: ", reprojError[1][0])

            # Export results csv
            csvwriter.writerow([loopRound, timestampOpenCV[loopRound],
                                rotVectors[0][0][0], rotVectors[0][1][0], rotVectors[0][2][0],
                                rotVectors[1][0][0], rotVectors[1][1][0], rotVectors[1][2][0], reprojError[0][0], reprojError[1][0]])

        # Display the current frame
        cv2.imshow('Playback', frame)  # Display the current frame

        # Export results video
        resultVid.write(frame)  # Incrementing filename

        # Press Q to stop video playback
        if cv2.waitKey(1) == ord('q'):
            break

        loopRound += 1

# Release playback after each dictionary
recording.release()
resultVid.release()

