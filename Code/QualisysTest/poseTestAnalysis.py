# --------------------------------------- Libraries ---------------------------------------
import cv2                  # Show video with OpenCV
import cv2.aruco as aruco   # Simplification
import os                   # Check file-extension
import numpy                # Python Math
import csv                  # Data import
# --------------------------------------- Libraries ---------------------------------------

# ------------------ Import/Export Setup ------------------
# Directory to save video 
dir = r'/home/thomaz/poseValidationTest'
os.chdir(dir)

# Test number for easy change
testNr = 4

tolerance = 0.1 # meters

# Plotting variables
#timeQTMplot = []
#timeCVplot = []
#timePlot = []
#xQTM,yQTM,zQTM = [],[],[]
#xCV,yCV,zCV    = [],[],[]

# ------------------ Import/Export Setup ------------------

rotVectors = []
transVectors = []
reprojError = []
startTime = 0
last10mag = []
detectedStart = False

# Import recording
recording = cv2.VideoCapture(f'qualisysTest_{testNr}.avi')

# Import timestamps from D435 recording
fileOpenCV = open(f'ExportedAruco_{testNr}.csv')
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
fileQTM = open(f'qualisysTest{testNr}_6D.tsv')
QTMreader = csv.reader(fileQTM, delimiter="\t")
QTMdata = []
for row in QTMreader:
    QTMdata.append(row)

#### Translation ####
# Extract all rows of columns 3 to 5 (excluding headers)
transQTM = [row[5:8] for row in QTMdata[14:]]
transQTM = numpy.array(transQTM, dtype=float)/1000.0 # millimeters -> meters

# Measured physical distances from RGB camera to Qualisys L-Frame
pTransVec = numpy.array([0.192, 0.144, 1.564])   # meters
pTransArr = numpy.tile(pTransVec, (len(transQTM), 1))

# Correct translation
transQTM = pTransArr - transQTM
#print("\nCorrected translation: ", transQTM)
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
        #print("i from loop: ", i)
        startTimeQTM = timeQTM[i]
        startIterQTM = iterQTM[i] # used?
        detectedStartQTM = True
#print("\nQTM start time: ", startTimeQTM)
#print("\nQTM iter start: ", startIterQTM)
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
rotElementsQTM = [row[12:21] for row in QTMdata[14:]]
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
#print("\nRotmatQTM: ", rotMatQTM)


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

#rounds = []

with open(filename,'a') as csvfile:
    # Set writer for loop
    csvwriter = csv.writer(csvfile)
        
    # From 0 to end(iterOpenCV)
    for loopRound in range(len(iterOpenCV)):
    
        ########################## THIS IS DONE INSIDE DETECTION #########################

        # Create transVectors
        print("\nloopRound: ", loopRound)
        transVector0 = numpy.array(dataOpenCV[loopRound+1][2:5], dtype=numpy.float32)
        transVector1 = numpy.array(dataOpenCV[loopRound+1][5:8], dtype=numpy.float32)
        # Create rotVectors
        rotVector0 = numpy.array(dataOpenCV[loopRound+1][8:11], dtype=numpy.float32)
        rotVector1 = numpy.array(dataOpenCV[loopRound+1][11:14], dtype=numpy.float32)

        #print("\ntransVector0: ", transVector0)
        #print("\nrotVector0: ", rotVector0)

        # Save current magnitude
        curTransMag = numpy.linalg.norm(transVector0)

        #### START DETECTION ####
        if (not detectedStart) and (len(last10mag) == 10) and (abs(last10mag[0] - curTransMag) > tolerance):
            #print("loopRound: ", loopRound)
            startTime = timestampOpenCV[loopRound]
            startLoop = loopRound
            #startIter = iterOpenCV[loopRound-1]
            #print("\nLoopRound: ", loopRound)
            #print("startIter: ", startIter)
            #print("startTime: ", startTime)
            detectedStart = True
        
        # Save last 10 frames magnitude
        last10mag.append(curTransMag)
        if (len(last10mag) > 10):
            last10mag.pop(0)

        curTime = timestampOpenCV[loopRound] - startTime

        # Match closest timestamps
        if detectedStart:
            # QTM time match - i will be saved when break
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
            rotMat0, _ = cv2.Rodrigues(rotVector0)
            rotMat1, _ = cv2.Rodrigues(rotVector1)
            
#                if not ((transVector0[0] == 0.0) and (transVector0[1] == 0.0) and (transVector0[2] == 0.0)):
#                    rotMat0, _ = cv2.Rodrigues(rotVector0)
#                    #print("\nRotMat0: ", rotMat0)
#                else:
#                    rotMat0 = numpy.zeros((3, 3), dtype=numpy.float32)
#                if not ((transVector1[0] == 0.0) and (transVector1[1] == 0.0) and (transVector1[2] == 0.0)):
#                    rotMat1, _ = cv2.Rodrigues(rotVector1)
#                else:
#                    rotMat1 = numpy.zeros((3, 3), dtype=numpy.float32)

        #print("\nRotMat0Element: ", rotMat0[1][2]) # [row][column]

            #print("\ni for csvwriter QTM: ", i)
            # Export time and translation vectors
            csvwriter.writerow([curTime, transQTM[i][0], transQTM[i][1], transQTM[i][2],
                                transVector0[0], transVector0[1], transVector0[2],
                                #transVector1[0], transVector1[1], transVector1[2],
                                rotMat0[0][0], rotMat0[0][1], rotMat0[0][2],
                                rotMat0[1][0], rotMat0[1][1], rotMat0[1][2],
                                rotMat0[2][0], rotMat0[2][1], rotMat0[2][2],
                                rotMat1[0][0], rotMat1[0][1], rotMat1[0][2], 
                                rotMat1[0][1], rotMat1[1][1], rotMat1[1][2], 
                                rotMat1[0][2], rotMat1[2][1], rotMat1[2][2], 
                                rotMatQTM[i][0][0], rotMatQTM[i][0][1], rotMatQTM[i][0][2],
                                rotMatQTM[i][1][0], rotMatQTM[i][1][1], rotMatQTM[i][1][2],
                                rotMatQTM[i][2][0], rotMatQTM[i][2][1], rotMatQTM[i][2][2],])

        ########################## THIS IS DONE INSIDE DETECTION #########################
        #rounds.append(loopRound)

print("\nStartOpen: ", startTime)
print("\nStartQTM: ", startTimeQTM)
print("\nStartLoop: ", startLoop)

#print("\nLoops: ", rounds)