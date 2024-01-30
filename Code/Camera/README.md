# Depth Camera Source Codes
_All source codes in this folder are for Intel Realsense D435 with stereovision, RGB module_and infrared light._
## Current Folder
### screenshotCamera.py
- Live camera reading
- s = screenshot, q = quit

### cameraCalibration.py
- Used for calibrating the camera
- __intrinsic parameters:__ matrix coefficients and distortion coefficients

### arucoDetection.py
- Simple detection of ArUco markers
- Display text and border

### arucoPoseEstimation.py
- Pose estimation of markers
- Builds on arucoDetection.py

## Screenshots Folder
- Screenshots from screenshotCamera.py will end here

## TestFiles
### depthCameraTest.py
- live RGB and depth reading using pyrealsense2 library
- from Nick DiFilippo: https://github.com/nickredsox/youtube/blob/master/Robotics/realsense.py


