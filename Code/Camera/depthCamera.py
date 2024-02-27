import cv2                  # OpenCV
import pyrealsense2 as rs   # RealSense wrapper
import numpy                # Python math
import os                   # For path

# Depth Camera connection
pipe = rs.pipeline()
config = rs.config()
height = 480
width = 848
fps = 60
alpha = 0.3

# Recursive Filter
alpha = 0.5
depthPrev = 0

# Depth reading configuration
cursor = (int(width/2), int(height/2))  # Pixels
curSize = 3                             # Pixels
curThick = 1                            # Pixels
curColor = (255, 255, 255)              # bgr

# Change directory for screenshot
os.chdir(r'/home/thomaz/MAS306_Drone_G12/Code/Camera/Screenshots')

# Depth Stream
config.enable_stream(rs.stream.depth, width, height, rs.format.z16, fps) # (streamType, xRes, yRes, format, fps)
# Infrared Stream
config.enable_stream(rs.stream.infrared, width, height, rs.format.y8, fps) # (streamType, xRes, yRes, format, fps)
pipe.start(config)

while(True):
    
    # Collect frames from camera
    frame = pipe.wait_for_frames()
    frame = frame.as_frameset().get_depth_frame()

    # Convert to numpy array
    image = numpy.asanyarray(frame.get_data())

    # Read distance at cursor
    depth = image.item(cursor[1], cursor[0])
    
    # Recursive Filter
    depth = depth*alpha + (1-alpha)*depthPrev
    depth = round(depth, 2)
    print("\nDepth Reading: ", depth)
    print(" [mm]\n")
    depthPrev = depth

    # Add color coding
    image = cv2.applyColorMap(cv2.convertScaleAbs(image, alpha=alpha), cv2.COLORMAP_TURBO)

    # Display cursor
    cv2.circle(image, cursor, curSize, curColor, thickness=curThick)

    # Display the current frame
    cv2.imshow('LiveReading', image)

    # Store key pressed during 1 [ms] delay
    pressedKey = cv2.waitKey(1)

    # S to take screenshot
    if pressedKey == ord('s'):
        cv2.imwrite(filename="screenshotDepth.jpg", img=image)  # Incrementing filename
        print('Screenshot successful!')

    # P to pause
    elif pressedKey == ord('p'):    # Press P to pause
        cv2.waitKey(-1)

    # Q to stop stream
    elif pressedKey == ord('q'):
        break

pipe.stop()             # Stop recording
cv2.destroyAllWindows() # Free resources 