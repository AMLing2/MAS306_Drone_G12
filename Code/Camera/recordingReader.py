import pyrealsense2 as rs   # stream configuration
import cv2 as cv            # Show video with OpenCV
import argparse             # Argument Parsing
import os.path              # Check file-extension
import numpy

# Create pipeline and config object
pipe = rs.pipeline()
cfg = rs.config()

# Size of window to display recording
displaySize = (960, 540)

# File Path
bag_file_path = r'/home/thomaz/Recordings/recordedVideo.bag'

# Enable playback from the specified bag file
cfg.enable_device_from_file(bag_file_path)

# Start pipeline with configuration
pipe.start(cfg)

while(True):
    
    frame = pipe.wait_for_frames() # waits for and collects all frames from camera (depth, color, etc)
    color_frame = frame.get_color_frame()
    color_image = numpy.asanyarray(color_frame.get_data())

    displayWindow = cv.resize(color_image, displaySize)

    cv.imshow('LiveReading', displayWindow)     # Display the current frame
    
    # Press Q to stop video playback
    if cv.waitKey(1) == ord('q'):
        break

pipe.stop()             # Stop recording
cv.destroyAllWindows() # Free resources 

