# --------------------------------------- Libraries ---------------------------------------
import cv2                  # Show video with OpenCV
import cv2.aruco as aruco   # Simplification
import os                   # Check file-extension
import numpy                # Python Math
import csv                  # Data import
#import pandas
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

# ------------------ Import/Export Video Setup ------------------
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
testNr = 6

tolerance = 0.1 # meters
# ------------------ Import/Export Video Setup ------------------

# Create object for parameters
arucoParams = aruco.DetectorParameters()

# Set dictionary for the markers
arucoParams     = aruco.DetectorParameters()
arucoDictionary = aruco.getPredefinedDictionary(aruco.DICT_5X5_50)

# Video number for filename
vidNr = 0

rotVectors = []
transVectors = []
reprojError = 0
loopRound = 1
markerFrames = 0
startTime = 0
last10mag = []
detectedStart = False

# Start new recording
resultVid = cv2.VideoWriter(f'resultsPoseTest_{testNr}.avi', fourcc, fps, (w,h))

# Import recording
recording = cv2.VideoCapture(f'qualisysTest_{testNr}.avi')

# Import timestamps from D435 recording
fileOpenCV = open(f'cameraTimestamps_{testNr}.csv')
timestampReader = csv.reader(fileOpenCV)
timestampOpenCV = []
for row in timestampReader:
    timestampOpenCV.append(row)

print("timestamps: ", timestampOpenCV[0])

# Import data from Qualisys
fileQTM = open(f'qualisysData_{testNr}.tsv')
QTMreader = csv.reader(fileQTM, delimiter="\t")
QTMdata = []
for row in QTMreader:
    QTMdata.append(row)

# Extract translation and rotation
transVectorsQTM = []
rotMatQTM = []

# Extract all rows of column 3 to 5 (excluding headers)
transQTM = [row[3:6] for row in QTMdata[14:]]
transQTM = numpy.array(transQTM, dtype=float)/1000.0 # millimeters -> meters

# Measured physical distances from RGB camera to Qualisys L-Frame
pTransVec = numpy.array([0.177, 0.129, 1.524])   # meters
pTransArr = numpy.tile(pTransVec, (len(transQTM), 1))

# Correct translation
transQTM = pTransArr - transQTM
print("\nCorrected translation: ", transQTM)

detectedStartQTM = False
last10magQTM = []
for i, row in enumerate(transQTM):

    #print("\niteration: ", i)
    curTransMagQTM = numpy.linalg.norm(transQTM[i])

    last10magQTM.append(curTransMagQTM)
    if (len(last10magQTM) > 10):
        last10magQTM.pop(0)
    print("\nLast10: ", last10magQTM)

    if (not detectedStartQTM) and (len(last10magQTM) == 10) and (abs(curTransMagQTM - last10magQTM[0]) > tolerance):
        startTimeQTM = QTMdata[i][1]
        detectedStartQTM = True
print("\nQTM start time: ", startTimeQTM)


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
            print("\nTranslation Vectors: ", transVectors)

            # Draw marker axes
            cv2.drawFrameAxes(frame, cameraMatrix=cameraMatrix,
                            distCoeffs=distortionCoefficients, rvec=rotVectors[0], tvec=transVectors[0], length=axesLength)
            
            # Print Current marker ID
#            print("\nCurrent ID: ", markerID)

            # Check for start movement
#            if (len(last10trans) == 10):
#                print("curMag: ", numpy.linalg.norm(transVectors[0]))
#                print("prevMag: ", numpy.linalg.norm(last10trans[0]))
            curTransMag = numpy.linalg.norm(transVectors[0])


            if (not detectedStart) and (len(last10mag) == 10) and (abs(last10mag[0] - curTransMag) > tolerance):
                #print("loopRound: ", loopRound)
                startTime = timestampOpenCV[loopRound][1]
                print("LoopRound: ", loopRound)
                print("start time: ", startTime)
                detectedStart = True

            # 
            last10mag.append(curTransMag)
            if (len(last10mag) > 10):
                last10mag.pop(0)

#            prevTransVec = transVectors[0]
            markerFrames += 1

        # Display the current frame
        cv2.imshow('Playback', frame)  # Display the current frame
                
        # Export results video
        resultVid.write(frame)  # Incrementing filename

    # Press Q to stop video playback
    if cv2.waitKey(1) == ord('q'):
        break

    loopRound += 1
    print("\nStartOpen: ", startTime)

# Release playback after each dictionary
recording.release()
resultVid.release()