import cv2                  # OpenCV
import pyrealsense2 as rs   # RealSense Wrapper
import numpy                # Python Math
import os                   # For path

# ------------------ Recording Setup ------------------
# Directory to save video 
dir = r'/home/thomaz/Recordings'
os.chdir(dir)

# Four-Character Code (File format)
fourcc = cv2.VideoWriter_fourcc(*'MJPG')

# Image Transfer Rate
fps = 60

# Image size: (horizontal, vertical) [pixels]
size = (848, 480)

# File Configuraiton for recording
recording = cv2.VideoWriter('recording.avi', fourcc, fps, size)
# ------------------ Recording Setup ------------------

# Depth Camera connection
pipe = rs.pipeline()
cfg = rs.config()

# Setup Stream
cfg.enable_stream(rs.stream.color, size, rs.format.bgr8, fps) # (streamType, xRes, yRes, format, fps)
pipe.start(cfg)

while(True):
    # Extract and convert frame
    frame = pipe.wait_for_frames() # waits for and collects all frames from camera (depth, color, etc)
    color_frame = frame.get_color_frame()
    color_image = numpy.asanyarray(color_frame.get_data())

    # Save frame to file
    recording.write(color_image)

    # In case of too large size to display
    #color_image = cv2.resize(color_image, displaySize)

    # Display the current frame
    cv2.imshow('LiveReading', color_image)

    # Press Q to stop recording
    if cv2.waitKey(1) == ord('q'):
        break

recording.release()
pipe.stop()             # Stop recording
cv2.destroyAllWindows() # Free resources