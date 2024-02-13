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
- File format: .bag

### recordingReader.py
- For reading the recorded videos from recordingCamera.py
- File path is local directory
- File format: .bag

### screenshotCamera.py
- For capturing screenshots from the camera stream
- Press S to capture the current frame
- Press Q to stop the stream
- The name of the screenshot will be numbered from the amount of files in the Screenshots folder


## Screenshots

- From screenshotCamera.py
- screenshot_0 = bgra8
- screenshot_1 = bgr8
- screenshot_2 = rgba8
- screenshot_3 = rgb8

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

- <span style="color:grey">Screenshots directory - frames save here
- <span style="color:grey">screenshotCamera.py - capture single frames
- <span style="color:grey">.pkl files - Pickle files for calibration testing export
- <span style="color:grey">cameraCalibration.py - Calibration testing.
