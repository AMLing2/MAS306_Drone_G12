# Inspired by Nicolai Nielsen tutorial: 
    # https://www.youtube.com/watch?v=3h7wgR5fYik
    # https://github.com/niconielsen32/CameraCalibration/blob/main/calibration.py

# --------------------------------------- Libraries ---------------------------------------
import cv2
import numpy
import glob         # For importing images
import pickle       # For export/import (of calibration results)
import os           # For selecting which image to display

# Don't need to import pyrealsense2 as we are using from screenshotCamera.py
# --------------------------------------- Libraries ---------------------------------------


# Setup variables
cameraRes = (848, 480) # [pixels] from datasheet
chessVertices = (13, 9)  # rows, columns
chessSquareSize = 19     # [mm]
imagePoints = []         # 2D
worldPoints = []         # 3D list. Stores the chessboard template for calibration, expanding 
iterStop = 30
cornerTolerance = 0.001
alpha = 1                # Scaling parameter for New Camera Matrix. 0 = max undistortion, 1 = min undistortion

# Load images
images = glob.glob('calibrationCaps/*.jpg')

# Termination Criteria for subpixels/corners
criteria = ( (cv2.TERM_CRITERIA_EPS + cv2.TermCriteria_MAX_ITER), iterStop, cornerTolerance)
winSize = (11, 11)      # OpenCV: "Size(5,5) , then a (5∗2+1)×(5∗2+1) = 11×11"
zeroZone = (-1, -1)     # Deadzone to avoid singularities. (-1,-1) = turned off

# Initialization for World Points (3D) for chessboard. 
worldPointsTemplate = numpy.zeros( ( (chessVertices[0]*chessVertices[1]), 3), numpy.float32 )        # Array (of zeros) with x,y,z coords for each vertex
worldPointsTemplate[:, :2] = numpy.mgrid[ 0:chessVertices[0], 0:chessVertices[1]].T.reshape(-1,2) # Coords for vertices [x_n, y_n, 0]. z=0 since flat
worldPointsTemplate *= chessSquareSize                                                         # Convert to [mm]

# ============================================ Corner Detection and Display ============================================

for image in images: # Iterates through images
    
    img = cv2.imread(image)                      # Read image from current index
    grey = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY) # Convert to greyscale

    ret, corners = cv2.findChessboardCorners(grey, chessVertices, corners=None) # none so Python auto-allocates memory for corners array

    # Want more accurate corner detection - subpixel algorithm
    if ret == True:
        
        # Store corners with whole- and subpixel coordinates
        worldPoints.append(worldPointsTemplate) # Expand list with the template
        imagePoints.append(corners)             # Expand list with detected corners
        subPixelCorners = cv2.cornerSubPix(grey, corners, winSize, zeroZone, criteria)
        
        # Display the current image with corner detection
        cv2.drawChessboardCorners(img, chessVertices, subPixelCorners, ret)
        cv2.imshow('Current Image', img)
        cv2.waitKey(10)   # Delay 1 [second]

cv2.destroyAllWindows()

# ============================================ Corner Detection and Display ============================================

# ================================================== Calibration  ==================================================

# Get Camera Matrix and Distortion Coefficients from openCV method
ret, cameraMatrix, distortionCoeffs, rVecs, tVecs = cv2.calibrateCamera(worldPoints, imagePoints, chessVertices, None, None) # None: auto-allocates memory

# Export Results
pickle.dump( (cameraMatrix, distortionCoeffs, rVecs, tVecs), open("calibrations.pkl", "wb")) # wb = Write Binary
pickle.dump( (cameraMatrix)                                , open("cameraMatrix.pkl", "wb"))
pickle.dump( (distortionCoeffs)                            , open("distortionCoefficients.pkl", "wb"))

# Display Results
print("\nRMS re-projection error: ", ret) # [pixels]
print("\nCamera Matrix:\n", cameraMatrix)
print("\nDistortion Coefficients:\n", distortionCoeffs)
print("\nRotation Vectors:\n", rVecs)
print("\nTranslation Vectors:\n", tVecs)

# For selecting which image to display
dir = r'/home/thomaz/MAS306_Drone_G12/Code/Camera/calibrationCaps'
os.chdir(dir)
#img = cv2.imread('screenshot_frameNr749.jpg') # <-------------------------- CHANGE IMAGE TO DISPLAY HERE

# Get Optimized Camera Matrix. (will reduce black borders from undistortion)
width, height = img.shape[:2] # Extract only height and width, not color channel
newCameraMatrix, regionOfInterest = cv2.getOptimalNewCameraMatrix(cameraMatrix, distortionCoeffs, (width, height), alpha)

# --------------------------------------- Undistortion ---------------------------------------

cameraMatrixActual = numpy.array([
    [613.037048339844,          0,         429.841949462891],    # [f_x, 0.0, c_x] used principal points 
    [  0,               612.738342285156,  237.866897583008],    # [0.0, f_y, c_y] as optical center points
    [  0,                       0,                1.0      ] ])  # [0.0, 0.0, 1.0]

undistortedImg = cv2.undistort(img, cameraMatrixActual, distortionCoeffs, None, newCameraMatrix)

#x, y, width, height = regionOfInterest
#undistortedImg = undistortedImg[ y:y+width, x:x+height ]
#print("\nx:", x)
#print("\ny:", y)
#print("\nw:", width)
#print("\nh:", height)

# Display images
while(True):
    
    # Display Images
    cv2.imshow('Image: Unprocessed', img)
    cv2.imshow('Image: Undistorted w/Camera Matrix', undistortedImg)
    #cv2.imshow('Image: Undistorted w/Camera Matrix', undistortedImg)
    
    # Stop or save images
    keyPressed = cv2.waitKey(1)
    if keyPressed == ord('s'):
        screenshot = cv2.imwrite(filename=f"result_Calibration.jpg", img=undistortedImg) # solution inspired by azro
    elif keyPressed == ord('q'):
        break
    



# --------------------------------------- Undistortion ---------------------------------------


# ================================================== Calibration  ==================================================