# PyImageSearch: https://pyimagesearch.com/2020/12/21/detecting-aruco-markers-with-opencv-and-python/

# --------------------------------------- Libraries ---------------------------------------
import cv2
import cv2.aruco as aruco # For simplification
import pyrealsense2 as rs
import numpy
# --------------------------------------- Libraries ---------------------------------------

# ------------------- Constant variables for simple changes -------------------
#markerColor = (255, 255, 0) # Corner color is set to be contrast to border color

font = cv2.FONT_HERSHEY_SIMPLEX
fontColor = (200, 20, 200)
fontScale = 0.5
fontThickness = 1
axesLength = 0.01

markerSize = 0.051 # Length of ArUco marker sides [m]

# Imported from Camera Calibration
    # For 848x480
cameraMatrix = numpy.array([
    [613.037048339844,          0,         429.841949462891],    # [f_x, 0.0, c_x] used principal points 
    [  0,               612.738342285156,  237.866897583008],    # [0.0, f_y, c_y] as optical center points
    [  0,                       0,                1.0      ] ])  # [0.0, 0.0, 1.0]
    # For 1920x1080
# cameraMatrix = numpy.array([
#     [1379.333496,   0,         973.144470],     # [f_x, 0.0, c_x] used principal points 
#     [  0,         1378.661255,  535.200500],    # [0.0, f_y, c_y] as optical center points
#     [  0,           0,           1.0      ] ])  # [0.0, 0.0, 1.0]

distortionCoefficients = numpy.array(
    [ 0.0, 0.0, 0.0, 0.0, 0.0]) # [k1, k2, p1, p2, k3]

print("\nCamera Matrix\n", cameraMatrix)
print("\nDistortion Coefficients\n", distortionCoefficients)
# ------------------- Constant variables for simple changes -------------------

# Set dictionary for the markers
arucoParams     = aruco.DetectorParameters()
#arucoDictionary = aruco.getPredefinedDictionary(aruco.DICT_7X7_50) # <-- Tip from chatGPT, Detector_get is old

# Set dictionary for the markers - TEMPORARY
dictList = numpy.array([
	aruco.DICT_6X6_50,
	aruco.DICT_7X7_50,
	aruco.DICT_APRILTAG_36h10,
	aruco.DICT_APRILTAG_36h11
])

# Setup configuration and start pipeline stream
pipe = rs.pipeline()
config = rs.config()

config.enable_stream(rs.stream.color, 848, 480, rs.format.bgr8, 60) # (streamType, xRes, yRes, format, fps)
pipe.start(config)

transVector = [0, 0, 0]
while(True):
    
    # Frame Collection
    frame = pipe.wait_for_frames()          # collect frames from camera (depth, color, etc)
    color_frame = frame.get_color_frame()   # Extract RGB module frame
    color_image = numpy.asanyarray(color_frame.get_data()) # Convert to NumPy array

    for dict in dictList:
        arucoDictionary = aruco.getPredefinedDictionary(dict) # <-- Tip from chatGPT, Detector_get is old

        # Identification
        gray = cv2.cvtColor(color_image, cv2.COLOR_BGR2GRAY)   # Grayscale image
        corners, ids, rejectedImagePoints = aruco.detectMarkers(gray, arucoDictionary, parameters=arucoParams)

        # ------------------------------------ Pose Estimation ------------------------------------
        if len(corners) > 0:
            for i in range(0, len(ids)):
                
                rotVector, transVector, markerPoints = cv2.aruco.estimatePoseSingleMarkers(
                    corners[i], markerSize, cameraMatrix=cameraMatrix, distCoeffs=distortionCoefficients)
                
                cv2.drawFrameAxes(color_image, cameraMatrix=cameraMatrix,
                                distCoeffs=distortionCoefficients, rvec=rotVector, tvec=transVector, length=axesLength)
                
                magnitude = numpy.linalg.norm(transVector)
                magnitude = round(magnitude, 4)
                print("\nMagnitude: ", magnitude)
        # ------------------------------------ Pose Estimation ------------------------------------


    # Information display in terminal
    # print(f'\n[INFO] ArUco Marker ID:     {ids}') # Array with current IDs: [ [id1] [id2] ... [idn] ]
    # print('[INFO] Corners: ')
    # print(corners)  # Matrix with current corner coordinates [ x1, y1; x2, y2; x3, y3; x4, y4 ], [-||-], ...


    # --------------------- v Display ID of marker, by pyImageSearch v ---------------------
    if len(corners) > 0:    # So iteration makes sense
        
        ids = ids.flatten() # Remove square brackets for text
        
        for (markerCorner, markerID) in zip(corners, ids): # alias on left, so can iterate in parallel
            # Extract corners
            corners = markerCorner.reshape(4,2)
            (topLeft, topRight, bottomRight, bottomLeft) = corners
            
            # convert each of the (x, y)-coordinate pairs to integers
            topRight    = (int(topRight[0]), int(topRight[1]))
            bottomRight = (int(bottomRight[0]), int(bottomRight[1]))
            bottomLeft  = (int(bottomLeft[0]), int(bottomLeft[1]))
            topLeft     = (int(topLeft[0]), int(topLeft[1]))

            IDtext = f'ID: {markerID}'

            # Display on the image
            cv2.putText(color_image, IDtext,(topLeft[0], topLeft[1] - 15), font, fontScale, fontColor, fontThickness)
    # --------------------- ^ Display ID of marker, by pyImageSearch ^ ---------------------
            
    # Display image
    cv2.imshow('LiveReading', color_image)

    # Loop breaker
    if cv2.waitKey(1) == ord('q'): # Press Q to stop
        break

pipe.stop()             # Stop recording
cv2.destroyAllWindows() # Free resources 