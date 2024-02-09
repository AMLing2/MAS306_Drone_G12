# ---- In order to numerate the videos, a solution provided by ezro at StackOverflow was used ----
# https://stackoverflow.com/questions/72329662/how-to-keep-cv2-imwrite-from-overwriting-the-image-files

import cv2
import pyrealsense2 as rs
import numpy
import os                   # For path

# Directory to save video 
dir = r'/home/thomaz/Recordings'
os.chdir(dir)

# Size of window to display recording
displaySize = (960, 540)

# Depth Camera connection
pipe = rs.pipeline()
cfg = rs.config()

cfg.enable_stream(rs.stream.color, 1920, 1080, rs.format.bgr8, 30) # (streamType, xRes, yRes, format, fps)
cfg.enable_record_to_file('recordedVideo.bag')

pipe.start(cfg)

while(True):
    
    frame = pipe.wait_for_frames() # waits for and collects all frames from camera (depth, color, etc)
    color_frame = frame.get_color_frame()
    color_image = numpy.asanyarray(color_frame.get_data())

    displayWindow = cv2.resize(color_image, displaySize)

    cv2.imshow('LiveReading', displayWindow)     # Display the current frame

    # Press Q to stop recording
    if cv2.waitKey(1) == ord('q'):
        break

pipe.stop()             # Stop recording
cv2.destroyAllWindows() # Free resources