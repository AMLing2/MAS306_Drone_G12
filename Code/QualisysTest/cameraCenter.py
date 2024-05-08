# --------------------------------------- Libraries ---------------------------------------
import cv2
import pyrealsense2 as rs
import numpy
# --------------------------------------- Libraries ---------------------------------------

screenHeight = 480   # pixels
screenWidth = 848    # pixels
fps = 60             # frames/second
cColor = (0, 0, 120) # bgr
cThick = 1           # pixels

# Setup configuration and start pipeline stream
pipe = rs.pipeline()
config = rs.config()

config.enable_stream(rs.stream.color, screenWidth, screenHeight, rs.format.bgr8, fps) # (streamType, xRes, yRes, format, fps)
pipe.start(config)

while(True):
    
    # Frame Collection
    frame = pipe.wait_for_frames()          # collect frames from camera (depth, color, etc)
    color_frame = frame.get_color_frame()   # Extract RGB module frame
    color_image = numpy.asanyarray(color_frame.get_data()) # Convert to NumPy array

    # Display crosshair horizontal then vertical        
    cv2.line(color_image, (0, int(screenHeight/2)), (screenWidth, int(screenHeight/2)), cColor, cThick)
    cv2.line(color_image, (int(screenWidth/2), 0), (int(screenWidth/2), screenHeight,), cColor, cThick)

    # Display image
    cv2.imshow('LiveReading', color_image)

    # Loop breaker
    pressedKey = cv2.waitKey(1)
    if pressedKey == ord('q'):      # Press Q to stop
        break
    elif pressedKey == ord('p'):    # Press P to pause
        cv2.waitKey(-1)

pipe.stop()             # Stop recording
cv2.destroyAllWindows() # Free resources 