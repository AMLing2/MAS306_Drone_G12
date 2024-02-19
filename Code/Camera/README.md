# Depth Camera Source Codes
_All source codes in this folder are for Intel Realsense D435 with stereovision, RGB module_and infrared light._

## Current Folder

### arucoDetection.py
- Simple detection of ArUco markers
- Display text and border

### arucoPoseEstimation.py
- Pose estimation of markers
- Display axes on marker
- Builds on arucoDetection.py

### depthCameraTest.py
- Live RGB and depth reading using pyrealsense2 library
- from Nick DiFilippo: https://github.com/nickredsox/youtube/blob/master/Robotics/realsense.py

### recordingCamera.py
- For recording videos with Intel Realsense D435 Camera.
- File path is local directory
- File format: .avi

### recordingReader.py
- For reading the recorded videos from recordingCamera.py
- Save dicitonary, rotation and traslation vectors in results folder

### screenshotCamera.py
- For capturing screenshots from the camera stream
- Press S to capture the current frame
- Press Q to stop the stream
- The name of the screenshot will be numbered from the amount of files in the Screenshots folder


## Screenshots

- From screenshotCamera.py
- screenshot_0 = color bgra8 1920x1080
- screenshot_1 = color bgr8 1920x1080
- screenshot_2 = color rgba8 1920x1080
- screenshot_3 = color rgb8 1920x1080
- screenshot_4 = ArUco tacking
- screenshot_5 = color y8 848x480
- screenshot_6 = infrared y8 848x480 25fps
- screenshot_7-saved = infrared y16 1280x800 25fps
- screenshot_8-saved = color y16 848x480 60fps
- screenshot_9-saved = color yuyv 848x480 60fps
- screenshot_10 = depth z16 848x480 60fps colormapped
- screenshot_11 = depth z16 848x480 60fps not mapped
- screenshot_12-saved = depth z16 848x480 60fps not mapped


## Results

### RSenumerateDevices.txt
- Listed distortion model
- Copy-pasted from terminal
- Requires SDK

### RealSenseViewerIntrinsics.json
- Listed intrinsic parameters from Intel.RealSense.Viewer.exe on Windows

### markerTestResults.csv
- List of results from recordingReader.py
- Dictionary, rotation and translation vectors

---
---

### <span style="color:grey">[Discontinued/Irrelevant] TestFiles

- <span style="color:grey">Screenshots directory - frames save here
- <span style="color:grey">screenshotCamera.py - capture single frames
- <span style="color:grey">.pkl files - Pickle files for calibration testing export
- <span style="color:grey">cameraCalibration.py - Calibration testing.
- <span style="color:grey">recordingReaderAll.py - Display all markers at once
