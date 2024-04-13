import cv2                  # OpenCV
import pyrealsense2 as rs   # RealSense Wrapper
import numpy                # Python Math
import os                   # For path
import csv
import time
# ------------------ Recording Setup ------------------
# Directory to save video 
dir = r'/home/thomaz/poseValidationTest'
os.chdir(dir)

# Four-Character Code (File format)
fourcc = cv2.VideoWriter_fourcc(*'MJPG')

# Image Transfer Rate
fps = 60

# Image size [pixels]
w = 848 # Width
h = 480 # Height

# File Configuraiton for recording
items = os.listdir(dir)
recNum = round(len(items)/2, None)
recording = cv2.VideoWriter(f'qualisysTest_{recNum}.avi', fourcc, fps, (w,h))
# ------------------ Recording Setup ------------------

# Depth Camera connection
pipe = rs.pipeline()
cfg = rs.config()

# Setup csv file
filename = f"cameraTimestamps_{recNum}.csv" 
fields = ['Frame','Timestamp']

# Column names
with open(filename, 'w') as csvfile:
		csvwriter = csv.writer(csvfile)
		csvwriter.writerow(fields)

# Setup Stream
cfg.enable_stream(rs.stream.color, w, h, rs.format.bgr8, fps) # (streamType, xRes, yRes, format, fps)
pipe.start(cfg)
n = 1

with open(filename,'a') as csvfile:
    # Set writer for loop
    csvwriter = csv.writer(csvfile)
    startTime = time.monotonic()
    while(True):
        # Extract and convert frame
        frame = pipe.wait_for_frames() # waits for and collects all frames from camera (depth, color, etc)
        csvwriter.writerow([n,time.monotonic()-startTime])
        n += 1

        color_frame = frame.get_color_frame()
        color_image = numpy.asanyarray(color_frame.get_data())

        # Save frame to file
        if True:
            recording.write(color_image)

        # Display the current frame
        cv2.imshow('LiveReading', color_image)

        # Press Q to stop recording
        if cv2.waitKey(1) == ord('q'):
            break

recording.release()
pipe.stop()             # Stop recording
cv2.destroyAllWindows() # Free resources