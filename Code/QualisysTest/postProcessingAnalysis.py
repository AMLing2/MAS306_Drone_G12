# --------------------------------------- Libraries ---------------------------------------
import cv2                  # Show video with OpenCV
import cv2.aruco as aruco   # Simplification
import os                   # Check file-extension
import numpy                # Python Math
import csv                  # Data import
import matplotlib.pyplot    # Plotting
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
dir = r'/home/thomaz/BackupsMAS306/WorkingExportMATLAB_15-4-2024'
os.chdir(dir)

# Four-Character Code (File format)
fourcc = cv2.VideoWriter_fourcc(*'MJPG')

# Image Transfer Rate
fps = 60

# Image size [pixels]
w = 848 # Width
h = 480 # Height

# Test number for easy change
testNr = 12

tolerance = 0.1 # meters

# Plotting variables
#timeQTMplot = []
#timeCVplot = []
#timePlot = []
#xQTM,yQTM,zQTM = [],[],[]
#xCV,yCV,zCV    = [],[],[]

# ------------------ Import/Export Setup ------------------

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
loopRound = 0
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

# ------------ Qualisys Data ---------------
### Import data from Qualisys ###
fileQTM = open(f'qualisysData_{testNr}.tsv')
QTMreader = csv.reader(fileQTM, delimiter="\t")
QTMdata = []
for row in QTMreader:
    QTMdata.append(row)

#### Translation ####
# Extract all rows of columns 3 to 5 (excluding headers)
transQTM = [row[3:6] for row in QTMdata[14:]]
transQTM = numpy.array(transQTM, dtype=float)/1000.0 # millimeters -> meters

# Measured physical distances from RGB camera to Qualisys L-Frame
pTransVec = numpy.array([0.192, 0.144, 1.564])   # meters
pTransArr = numpy.tile(pTransVec, (len(transQTM), 1))

# Correct translation
transQTM = pTransArr - transQTM
print("\nCorrected translation: ", transQTM)
#print("\nExported variable: ", transQTM[2][1])

#### Start Detection ####
detectedStartQTM = False
last10magQTM = []

# Time list
timeQTM = [row[1] for row in QTMdata[14:]]
timeQTM = numpy.array(timeQTM, dtype=float)
iterQTM = [row[0] for row in QTMdata[14:]]
iterQTM = numpy.array(iterQTM, dtype=int)

# Find start time
for i, row in enumerate(transQTM):

    #print("\niteration: ", i)
    curTransMagQTM = numpy.linalg.norm(transQTM[i])

    last10magQTM.append(curTransMagQTM)
    if (len(last10magQTM) > 10):
        last10magQTM.pop(0)
    #print("\nLast10: ", last10magQTM)

    if (not detectedStartQTM) and (len(last10magQTM) == 10) and (abs(curTransMagQTM - last10magQTM[0]) > tolerance):
        print("i from loop: ", i)
        startTimeQTM = timeQTM[i]
        startIterQTM = iterQTM[i] # used?
        detectedStartQTM = True
print("\nQTM start time: ", startTimeQTM)
print("\nQTM iter start: ", startIterQTM)
# Start value to be changed
correctTimeQTM = startTimeQTM

# Convert from strings
#timeQTM = [float(element) for element in timeQTM]
#print("\ntimeQTM: ", timeQTM)

# Difference list -------> starts @ 1 not 0 <--------
dTimeQTM = [timeQTM[i]-timeQTM[i-1] for i in range(1, len(timeQTM))]
#print("\ndTimeQTM: ", dTimeQTM)

#print("\nLen: ", len(timeQTM))
#print("\nLenDiff: ", len(dTimeQTM))


#### Rotation ####
# Extract all rows of columns 10 to 18 (excluding headers)
rotElementsQTM = [row[10:19] for row in QTMdata[14:]]
#print("\nRotmatQTM: ", rotElementsQTM)
rotElementsQTM = [[float(element) for element in sublist] for sublist in rotElementsQTM]

# Rotation Matrix for relating frames: Qualisys -> D435
rotMatQTM2OpenCV = numpy.array([
    [-1.0, 0.0,  0.0],
    [ 0.0, 1.0,  0.0],
    [ 0.0, 0.0, -1.0]
])

# Convert to new form and relate to D435 frame
rotMatQTM = []
for row in rotElementsQTM:
    matrix = numpy.array(row).reshape(3,3)
    matrix = matrix @ rotMatQTM2OpenCV # Convert orientation
    rotMatQTM.append(matrix)
# ------------ Qualisys Data ---------------
print("\nRotmatQTM: ", rotMatQTM)


# Setup csv file
filename = f"ExportedResults_{testNr}.csv" 
fields = ['Time','xQTM', 'yQTM', 'zQTM', 'xCV', 'yCV', 'zCV', 
          'v0R11','v0R12','v0R13','v0R21','v0R22','v0R23','v0R31','v0R32','v0R33',
          'v1R11','v1R12','v1R13','v1R21','v1R22','v1R23','v1R31','v1R32','v1R33',
          'qtmR11','qtmR12','qtmR13','qtmR21','qtmR22','qtmR23','qtmR31','qtmR32','qtmR33']

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

                # Save current magnitude
                curTransMag = numpy.linalg.norm(transVectors[0])

                #### START DETECTION ####
                if (not detectedStart) and (len(last10mag) == 10) and (abs(last10mag[0] - curTransMag) > tolerance):
                    #print("loopRound: ", loopRound)
                    startTime = timestampOpenCV[loopRound]
                    #startIter = iterOpenCV[loopRound-1]
                    print("\nLoopRound: ", loopRound)
                    #print("startIter: ", startIter)
                    print("startTime: ", startTime)
                    detectedStart = True

                # Save last 10 frames magnitude
                last10mag.append(curTransMag)
                if (len(last10mag) > 10):
                    last10mag.pop(0)

                curTime = timestampOpenCV[loopRound] - startTime

                # Match closest timestamps
                if detectedStart:
                    for i in range(len(timeQTM)):
                        if (0 < ((timeQTM[i] - startTimeQTM) - curTime)):
                            correctTimeQTM = timeQTM[i]
                            break

                    #print("\nCurrent Time: ", curTime)
                    #print("\nOpenCV Time: ", timestampOpenCV[loopRound])
                    #print("\nQTM Time: ", correctTimeQTM)

                    #print("\nRotVector: ", rotVectors[0])
                    #print("\nRotVectorElement: ", rotVectors[0][1][0])
                    
                    #print("\nrotmatQTM: ", rotMatQTM[i])
                    #print("\nrotmatQTMElement: ", rotMatQTM[i][0][1])

                    rotMat0, _ = cv2.Rodrigues(rotVectors[0])
                    rotMat1, _ = cv2.Rodrigues(rotVectors[1])

                    #print("\nRotMat0: ", rotMat0)
                    #print("\nRotMat0Element: ", rotMat0[1][2]) # [row][column]

                    # Export time and translation vectors
                    csvwriter.writerow([curTime, transQTM[i][0], transQTM[i][1], transQTM[i][2],
                                        transVectors[0][0][0], transVectors[0][1][0], transVectors[0][2][0], 
                                        rotMat0[0][0], rotMat0[0][1], rotMat0[0][2],
                                        rotMat0[1][0], rotMat0[1][1], rotMat0[1][2],
                                        rotMat0[2][0], rotMat0[2][1], rotMat0[2][2],
                                        rotMat1[0][0], rotMat1[0][1], rotMat1[0][2], 
                                        rotMat1[0][1], rotMat1[1][1], rotMat1[1][2], 
                                        rotMat1[0][2], rotMat1[2][1], rotMat1[2][2], 
                                        rotMatQTM[i][0][0], rotMatQTM[i][0][1], rotMatQTM[i][0][2],
                                        rotMatQTM[i][1][0], rotMatQTM[i][1][1], rotMatQTM[i][1][2],
                                        rotMatQTM[i][2][0], rotMatQTM[i][2][1], rotMatQTM[i][2][2],])

            # Display the current frame
            cv2.imshow('Playback', frame)  # Display the current frame
            
            # Export results video
            resultVid.write(frame)  # Incrementing filename

        # Press Q to stop video playback
        if cv2.waitKey(1) == ord('q'):
            break

        loopRound += 1

print("\nStartOpen: ", startTime)
print("\nStartQTM: ", startTimeQTM)

# Release playback after each dictionary
recording.release()
resultVid.release()