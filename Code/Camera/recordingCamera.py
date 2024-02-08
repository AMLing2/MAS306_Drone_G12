
# ---- In order to numerate the videos, a solution provided by ezro at StackOverflow was used ----
# https://stackoverflow.com/questions/72329662/how-to-keep-cv2-imwrite-from-overwriting-the-image-files

import cv2
import pyrealsense2 as rs
import numpy
import os                   # For path

# Directory to save video 
dir = r'/home/thomaz/MAS306_Drone_G12/Code/Camera/Recordings'
os.chdir(dir)

# Depth Camera connection
pipe = rs.pipeline()
config = rs.config()

config.enable_stream(rs.stream.color, 640, 480, rs.format.bgr8, 30) # (streamType, xRes, yRes, format, fps)
config.enable_record_to_file('recordedVideo.bag')

pipe.start(config)

i = 0

while(True):
    
    frame = pipe.wait_for_frames() # waits for and collects all frames from camera (depth, color, etc)
    color_frame = frame.get_color_frame()
    color_image = numpy.asanyarray(color_frame.get_data())

    cv2.imshow('LiveReading', color_image)     # Display the current frame

    keyPressed = cv2.waitKey(1) # Store key pressed during 1 [ms] delay

    if keyPressed == ord('q'):
        break
    i += 1

pipe.stop()             # Stop recording
cv2.destroyAllWindows() # Free resources 