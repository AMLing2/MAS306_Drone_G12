# Inspired by Nicolai Nielsen tutorial: 
    # https://www.youtube.com/watch?v=3h7wgR5fYik
    # https://github.com/niconielsen32/CameraCalibration/blob/main/calibration.py

# --------------------------------------- Libraries ---------------------------------------
import cv2          # OpenCV
import numpy        # Python Math
import glob         # For importing images
import os           # For export path

# Don't need to import pyrealsense2 as we are using from screenshotCamera.py
# --------------------------------------- Libraries ---------------------------------------

# Get the current working directory
curDir = os.getcwd()
# Create object for result export
resultDir = os.path.join(curDir, 'Results')

# Setup variables
cameraRes = (848, 480) # [pixels] from datasheet
chessVertices = (13, 9)  # rows, columns
chessSquareSize = 19     # [mm]
imagePoints = []         # 2D
worldPoints = []         # 3D list. Stores the chessboard template for calibration, expanding 

# Load images
images = glob.glob('calibrationCaps/*.jpg')

# Termination Criteria for subpixels/corners
iterStop = 30
cornerTolerance = 0.001
criteria = ( (cv2.TERM_CRITERIA_EPS + cv2.TermCriteria_MAX_ITER), iterStop, cornerTolerance)
winSize = (11, 11)      # OpenCV: "Size(5,5) , then a (5*2+1)x(5*2+1) = 11×11"
zeroZone = (-1, -1)     # Deadzone to avoid singularities. (-1,-1) = turned off

# Initialization for World Points (3D) for chessboard. 
worldPointsTemplate = numpy.zeros( ( (chessVertices[0]*chessVertices[1]), 3), numpy.float32 )        # Array (of zeros) with x,y,z coords for each vertex
worldPointsTemplate[:, :2] = numpy.mgrid[ 0:chessVertices[0], 0:chessVertices[1]].T.reshape(-1,2) # Coords for vertices [x_n, y_n, 0]. z=0 since flat
worldPointsTemplate *= chessSquareSize                                                         # Convert to [mm]

# ============================================ Corner Detection and Display ============================================

for image in images: # Iterates through images
    
    img = cv2.imread(image)                      # Read image from current index
    grey = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY) # Convert to greyscale

    ret, corners = cv2.findChessboardCorners(grey, chessVertices, None) # none so Python auto-allocates memory for corners array

    # Want more accurate corner detection - subpixel algorithm
    if ret == True:
        
        # Store corners with whole- and subpixel coordinates
        worldPoints.append(worldPointsTemplate) # Expand list with the template
        cv2.cornerSubPix(grey, corners, winSize, zeroZone, criteria)
        imagePoints.append(corners)             # Expand list with detected corners
        
        # Display the current image with corner detection
        cv2.drawChessboardCorners(img, chessVertices, corners, ret)
        cv2.imshow('Current Image', img)
        cv2.waitKey(250)   # Delay 250 [ms]

cv2.destroyAllWindows()

# ============================================ Corner Detection and Display ============================================

# ================================================== Calibration  ==================================================

# Get Camera Matrix and Distortion Coefficients from openCV method
ret, cameraMatrix, distortionCoeffs, rVecs, tVecs = cv2.calibrateCamera(worldPoints, imagePoints, cameraRes, None, None) # None: auto-allocates memory

# Display Results
print("\nRMS re-projection error: ", ret) # [pixels]
print("\nCamera Matrix:\n", cameraMatrix)
print("\nDistortion Coefficients:\n", distortionCoeffs)
# print("\nRotation Vectors:\n", rVecs)
# print("\nTranslation Vectors:\n", tVecs)

img = cv2.imread('calibrationCaps/screenshot_0.jpg') # <-------------------------- CHANGE IMAGE TO DISPLAY HERE

# --------------------------------------- Undistortion Display ---------------------------------------

undistortedImg = cv2.undistort(img, cameraMatrix, distortionCoeffs, None)

# Display images
while(True):
    
    # Display Images
    cv2.imshow('Image: Unprocessed', img)
    cv2.imshow('Image: Undistorted, not cropped', undistortedImg)
    
    # Stop or save images
    keyPressed = cv2.waitKey(1)
    if keyPressed == ord('s'):
        os.chdir(resultDir)
        screenshot = cv2.imwrite(filename=f"calibrationResultOriginal.jpg", img=img)
        screenshot = cv2.imwrite(filename=f"calibrationResultCropped.jpg", img=undistortedImg)
        print('Successfully captured resulting images!')
    elif keyPressed == ord('q'):
        break

# --------------------------------------- Undistortion Display ---------------------------------------


# ================================================== Calibration  ==================================================