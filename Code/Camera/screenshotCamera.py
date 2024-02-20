import cv2                  # OpenCV
import pyrealsense2 as rs   # RealSense wrapper
import numpy                # Python math
import os                   # For path

# Depth Camera connection
pipe = rs.pipeline()
config = rs.config()

config.enable_stream(rs.stream.color, 848, 480, rs.format.bgr8, 60) # (streamType, xRes, yRes, format, fps)

pipe.start(config)

# Frame counter
frameNr = 0

# Get the current working directory
curDir = os.getcwd()

# Screenshot directory
screenDir = os.path.join(curDir, 'Screenshots')

# Calibration screenshots directory
calibDir = os.path.join(curDir, 'calibrationCaps')

while(True):
    
    # Collect frames from camera (depth, color, IR)
    frame = pipe.wait_for_frames()

    # Toggle these three lines when changing config from (depth,color,IR)
    #frame = frame.get_infrared_frame()
    frame = frame.get_color_frame()
    #frame = frame.get_depth_frame()

    # Convert to numpy array
    image = numpy.asanyarray(frame.get_data())

    # Add color coding if using depth stream
    #image = cv2.applyColorMap(cv2.convertScaleAbs(image, alpha=1), cv2.COLORMAP_TURBO)

    # Display the current frame
    cv2.imshow('LiveReading', image)

    # Store key pressed during 1 [ms] delay
    keyPressed = cv2.waitKey(1)

    # S to take screenshot
    if keyPressed == ord('s'):
        os.chdir(screenDir) # Change directory
        # ---------- ChatGPT -----------------
        # Get the list of items (files and folders) inside the folder
        items = os.listdir(screenDir)
        # Count the number of items
        num_items = len(items)
        # ---------- ChatGPT -----------------
        cv2.imwrite(filename=f"screenshot_{num_items}.jpg", img=image)  # Incrementing filename
        print('Screenshot successful!')
    
    # C to capture for calibration
    if keyPressed == ord('c'):
        os.chdir(calibDir)  # Change this for screenshots or for calibrationr)
        cv2.imwrite(filename=f"screenshot_frame{frameNr}.jpg", img=image)  # Incrementing filename
        print('Calibration frame captured!')

    # Q to stop stream
    elif keyPressed == ord('q'):
        break
    frameNr += 1

pipe.stop()             # Stop recording
cv2.destroyAllWindows() # Free resources 