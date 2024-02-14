
# ---- In order to numerate the screenshots, a solution provided by ezro at StackOverflow was used ----
# https://stackoverflow.com/questions/72329662/how-to-keep-cv2-imwrite-from-overwriting-the-image-files

import cv2
import pyrealsense2 as rs
import numpy
import os                   # For path

# Directory to save image 
dir = r'/home/thomaz/MAS306_Drone_G12/Code/Camera/Screenshots'
os.chdir(dir)

# ---------- ChatGPT -----------------
# Get the list of items (files and folders) inside the folder
items = os.listdir(dir)
# Count the number of items
num_items = len(items)
# ---------- ChatGPT -----------------

# Depth Camera connection
pipe = rs.pipeline()
config = rs.config()

config.enable_stream(rs.stream.color, 848, 480, rs.format.bgr8, 30) # (streamType, xRes, yRes, format, fps)

pipe.start(config)

while(True):
    
    frame = pipe.wait_for_frames() # waits for and collects all frames from camera (depth, color, etc)
    color_frame = frame.get_color_frame()
    color_image = numpy.asanyarray(color_frame.get_data())

    cv2.imshow('LiveReading', color_image)     # Display the current frame

    keyPressed = cv2.waitKey(1) # Store key pressed during 1 [ms] delay

    if keyPressed == ord('s'):
        cv2.imwrite(filename=f"screenshot_{num_items}.jpg", img=color_image) # solution inspired by azro
        print('Screenshot successful!')
    elif keyPressed == ord('q'):
        break

pipe.stop()             # Stop recording
cv2.destroyAllWindows() # Free resources 