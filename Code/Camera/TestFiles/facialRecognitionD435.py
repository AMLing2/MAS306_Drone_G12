# From tutorials on youtube: 
    # Simple Face Detection in Python \\ https://www.youtube.com/watch?v=5cg_yggtkso
    # OpenCV Python Tutorial For Beginners 4 \\ https://www.youtube.com/watch?v=-RtVZsCvXAQ&t=871s
    # GeeksForGeeks for saving video \\ https://www.geeksforgeeks.org/saving-a-video-using-opencv/
    # Depth Camera Tutorial: https://www.youtube.com/watch?v=CmDO-w56qso

import pathlib # To access existing xml files for facial recognition
import cv2
import pyrealsense2 as rs
import numpy

    # Finding the xml file location
cascade_path = pathlib.Path(cv2.__file__).parent.absolute() / "data/haarcascade_frontalface_default.xml"
    #print(cascade_path)
classifier = cv2.CascadeClassifier(str(cascade_path))

# Depth Camera connection
pipe = rs.pipeline()
cfg = rs.config()

cfg.enable_stream(rs.stream.color, 640, 480, rs.format.bgr8, 30) # (streamType, xRes, yRes, bits, fps)

pipe.start(cfg)

while(True):
    frame = pipe.wait_for_frames() # waits for and collects all frames from camera (depth, color, etc)
    color_frame = frame.get_color_frame()

    color_image = numpy.asanyarray(color_frame.get_data())

    gray = cv2.cvtColor(color_image, cv2.COLOR_BGR2GRAY)

    # Library uses Haar Cascade Filter: https://docs.opencv.org/3.4/db/d28/tutorial_cascade_classifier.html
    faces = classifier.detectMultiScale(
        gray,
        scaleFactor=1.1,
        minNeighbors=5,
        minSize=(30,30),
        flags=cv2.CASCADE_SCALE_IMAGE
    )

    for (x, y, width, height) in faces:
        cv2.rectangle(color_image, (x, y), (x+width, y+height), (255, 255, 0), 2)


    cv2.imshow('FacialRecognition', color_image)     # Display the current frame

    if cv2.waitKey(1) == ord('q'): # user input (after 1 ms) == q
        break

pipe.stop()