import cv2                  # OpenCV
import pyrealsense2 as rs   # RealSense Wrapper
import numpy                # Python Math
import os                   # For path
import csv
import time
# ------------------ Recording Setup ------------------
# Directory to save video 
dir = r'/home/thomaz/Recordings/poseValidationTest'
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
recNum = len(items)
recording = cv2.VideoWriter(f'qualisysTest_{recNum}.avi', fourcc, fps, (w,h))
# ------------------ Recording Setup ------------------

# Depth Camera connection
pipe = rs.pipeline()
cfg = rs.config()
#setup csv file
filename - "cameraTimestamps.csv" 
fields = ['Frame','Timestamp']
with open(filename, 'w') as csvfile:
		csvwriter = csv.writer(csvfile)
		csvwriter.writerow(fields)

# Setup Stream
cfg.enable_stream(rs.stream.color, w, h, rs.format.bgr8, fps) # (streamType, xRes, yRes, format, fps)
pipe.start(cfg)
n = 1

while(True):
	# Extract and convert frame
	frame = pipe.wait_for_frames() # waits for and collects all frames from camera (depth, color, etc)
	with open(filename,'a') as csvfile:
		csvwriter = csv.writer(csvfile)
		csvwriter.writerow(n,time.time())
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
