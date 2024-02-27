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

### cameraCalibration.py
- Calibrates camera from screenshots in calibrationCaps
- Displays estimated camera matrix and distortion coefficients
- Displays undistortion

### depthCamera.py
- Live reading of the stereo vision
- Q for stop stream, P for pause
- Average value past 10 frames

### depthCameraTest.py
- Live RGB and depth reading using pyrealsense2 library
- from Nick DiFilippo: https://github.com/nickredsox/youtube/blob/master/Robotics/realsense.py

### markerTestAnalyzer.py
- For reading the recorded videos from recordingCamera.py
- Display computation time
- Display number of frames with data
- Display number of false positives

### markerTestVideoExport.py
- For playback and saving of video
- Only one dict at a time, all at once in TestFiles folder
- Saves video in local directory
- Videos are TBA in final project video, currently on google drive

### recordingCamera.py
- For recording videos with Intel Realsense D435 Camera.
- File path is local directory
- File format: .avi
- Final Test is 4, more speed and more angles.

### screenshotCamera.py
- For capturing frames from the camera stream
- Press S to take a screenshot
- Press C to capture current frame for calibration 
- Press Q to stop the stream
- Automatic directory change and numbering

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
- screenshot_13 = Dynamic Calibration Tool: Print Target

## calibrationCaps
- Screenshots captured for calibration
- from screenshotCamera.py ~ Remember to change save folder!
- Saved based on frames

## Results

### RSenumerateDevices.txt
- Listed distortion model
- Copy-pasted from terminal
- Requires SDK

### RealSenseViewerIntrinsics.json
- Listed intrinsic parameters from Intel.RealSense.Viewer.exe on Windows

---
---

### <span style="color:grey">[Discontinued/Irrelevant] TestFiles

- <span style="color:grey">Screenshots directory - old frames saved here
- <span style="color:grey">.pkl files - Pickle files for calibration testing export
- <span style="color:grey">recordingReaderAll.py - Display all markers at once
