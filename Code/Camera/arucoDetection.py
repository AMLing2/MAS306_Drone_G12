
import cv2
import cv2.aruco as aruco # For simplification
import pyrealsense2 as rs
import numpy


# Set dictionary for the markers
arucoParams     = aruco.DetectorParameters()
arucoDictionary = aruco.getPredefinedDictionary(aruco.DICT_6X6_1000) # Tips from GPT, Detector_get is old

vectorRotation    = None
vectorTranslation = None


# Setup configuration and start pipeline stream
pipe = rs.pipeline()
config = rs.config()

config.enable_stream(rs.stream.color, 640, 480, rs.format.bgr8, 30) # (streamType, xRes, yRes, format, fps)
pipe.start(config)

while(True):
    

    frame = pipe.wait_for_frames() # waits for and collects all frames from camera (depth, color, etc)
    color_frame = frame.get_color_frame()
    color_image = numpy.asanyarray(color_frame.get_data())

    # Identification
    gray = cv2.cvtColor(color_image, cv2.COLOR_BGR2GRAY) # Grayscale image
    corners, ids, rejectedImagePoints = aruco.detectMarkers(gray, arucoDictionary, parameters=arucoParams)

    # Draw outline of markers
    color_image = aruco.drawDetectedMarkers(color_image, corners, borderColor=(0,0,255))


    cv2.imshow('LiveReading', color_image)     # Display the current frame
    print('ArUco Marker ID: ')
    print(ids)
    print('Corners: ')
    print(corners)      # Matrix with coordinates of all 4 corners[ x1, y1; x2, y2; x3, y3; x4, y4 ]

    if cv2.waitKey(1) == ord('q'):
        break

pipe.stop()             # Stop recording
cv2.destroyAllWindows() # Free resources 