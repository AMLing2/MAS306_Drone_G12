
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

config.enable_stream(rs.stream.depth, 848, 480, rs.format.z16, 60) # (streamType, xRes, yRes, format, fps)

pipe.start(config)

while(True):
    
    # Collect frames from camera (depth, color, IR)
    frame = pipe.wait_for_frames()

    # - convert to specific frame
    #frame = frame.get_infrared_frame() # <-- Toggle these three lines when changing config from (depth,color,IR)
    #frame = frame.get_color_frame() # <-- Toggle these three lines when changing config from (depth,color,IR)
    frame = frame.get_depth_frame() # <-- Toggle these three lines when changing config from (depth,color,IR)

    # Convert to numpy array
    image = numpy.asanyarray(frame.get_data())

    # Add color coding if using depth stream
    image = cv2.applyColorMap(cv2.convertScaleAbs(image, alpha=1), cv2.COLORMAP_TURBO)

    # Display the current frame
    cv2.imshow('LiveReading', image)

    # Store key pressed during 1 [ms] delay
    keyPressed = cv2.waitKey(1)

    # Check what key was pressed
    if keyPressed == ord('s'): # Screenshot
        cv2.imwrite(filename=f"screenshot_{num_items}.jpg", img=image) # solution inspired by azro
        print('Screenshot successful!')
    elif keyPressed == ord('q'): # Stop stream
        break

pipe.stop()             # Stop recording
cv2.destroyAllWindows() # Free resources 