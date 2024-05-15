# Depth Camera Source Codes
_All source codes in this folder are for Intel Realsense D435 with stereovision, RGB module_and infrared light._

## [Folder] Current working directory

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

### computerVisionMain.py - Final version of visual pose estimation
- Main code for pose estimation implementation inside arena
- Final version after going to solePnP method ~~instead of estimatePoseSingleMarkers()~~
- Also includes communication after collaboration with infrastructure-responsible student

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

### markerTestVideoExport.py
- For playback and saving of video
- Only one dict at a time, all at once in TestFiles folder
- Saves video in local directory
- Videos are shown in final project video, also on google drive/zip

### poseMarkerDepthPreArena.py
- [DEPRECATED] Old file with estimatePoseSingleMarkers()
- Successor is computerVisionMain.py
- Color and Depth Stream (incorrect depth combination)
- Displays axes and crosshair on color and depth respectively
- Reads rVec, tVec and depth

### poseTransRot.py - Not included in report
- For testing with rotation transforamtions
- Trimmed poseMarkerDepth and added rotation matrices
- Rodrigues function for converting between vector and matrix
- Pitch roll and yaw matrices

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

## [Folder] Screenshots

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

## [Folder] calibrationCaps
- Screenshots captured for calibration
- from screenshotCamera.py
- Saved based on frames

## <span style="color:grey">[Discontinued/Irrelevant Folder] Results

### <span style="color:grey"> LiveReading_screenshot_20.02.2024
- <span style="color:grey"> From translation measurement outside arena

### <span style="color:grey"> calibrationResultOriginal & calibrationResultCropped
- <span style="color:grey"> From calibration for report

### <span style="color:grey"> RSenumerateDevices.txt
- <span style="color:grey"> Listed distortion model
- <span style="color:grey"> Copy-pasted from terminal
- <span style="color:grey"> Requires SDK

### <span style="color:grey"> RealSenseViewerIntrinsics.json
- <span style="color:grey"> Listed intrinsic parameters from Intel.RealSense.Viewer.exe on Windows

---
---

### <span style="color:grey">[Discontinued/Irrelevant] TestFiles

- <span style="color:grey">Screenshots directory - old frames saved here
- <span style="color:grey">.pkl files - Pickle files for calibration testing export
- <span style="color:grey">recordingReaderAll.py - Display all markers at once
