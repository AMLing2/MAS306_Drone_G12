# PyImageSearch: https://pyimagesearch.com/2020/12/21/detecting-aruco-markers-with-opencv-and-python/

# --------------------------------------- Libraries ---------------------------------------
import cv2                  # OpenCV
import cv2.aruco as aruco   # For simplification
import pyrealsense2 as rs   # RealSense Wrapper
import numpy                # Python Math
import os                   # Path Control
# --------------------------------------- Libraries ---------------------------------------

# ------------------- Constant variables for simple changes -------------------
markerColor = (255, 255, 0) # Corner color is set to be contrast to border color

font = cv2.FONT_HERSHEY_SIMPLEX
fontColor = (200, 20, 200)
fontScale = 0.5
fontThickness = 1
# ------------------- Constant variables for simple changes -------------------

# -------------------------------- Save Frame ---------------------------
# Directory to save image 
dir = r'/home/thomaz/MAS306_Drone_G12/Code/Camera/Screenshots'
os.chdir(dir)

# ---------- ChatGPT -----------------
# Get the list of items (files and folders) inside the folder
items = os.listdir(dir)
# Count the number of items
num_items = len(items)
# ---------- ChatGPT -----------------
# -------------------------------- Save Frame ---------------------------

# Set dictionary for the markers
arucoDictionary = aruco.getPredefinedDictionary(aruco.DICT_6X6_50)

# Setup configuration and start pipeline stream
pipe = rs.pipeline()
config = rs.config()

config.enable_stream(rs.stream.color, 848, 480, rs.format.bgr8, 60) # (streamType, xRes, yRes, format, fps)
pipe.start(config)


while(True):
    
    # Frame Collection
    frame = pipe.wait_for_frames()          # collect frames from camera (depth, color, etc)
    color_frame = frame.get_color_frame()   # Extract RGB module frame
    color_image = numpy.asanyarray(color_frame.get_data()) # Convert to NumPy array

    # Identification
    gray = cv2.cvtColor(color_image, cv2.COLOR_BGR2GRAY)   # Grayscale image
    corners, ids, rejectedImagePoints = aruco.detectMarkers(gray, arucoDictionary, parameters=aruco.DetectorParameters())

    # Information display in terminal
    print(f'\n[INFO] ArUco Marker ID:     {ids}') # Array with current IDs: [ [id1] [id2] ... [idn] ]
    print('[INFO] Corners: ')
    print(corners)  # Matrix with current corner coordinates [ x1, y1; x2, y2; x3, y3; x4, y4 ], [-||-], ...


    # Display outline of markers
    color_image = aruco.drawDetectedMarkers(color_image, corners, borderColor=markerColor)

    # --------------------- v Display ID of marker, by pyImageSearch v ---------------------
    if len(corners) > 0:    # So iteration makes sense
        
        ids = ids.flatten() # Remove square brackets for text
        
        for (markerCorner, markerID) in zip(corners, ids): # alias on left, so can iterate in parallel
            # Extract corners
            corners = markerCorner.reshape(4,2)
            (topLeft, topRight, bottomRight, bottomLeft) = corners
            
            # convert each of the (x, y)-coordinate pairs to integers
            topRight    = (int(topRight[0]), int(topRight[1]))
            bottomRight = (int(bottomRight[0]), int(bottomRight[1]))
            bottomLeft  = (int(bottomLeft[0]), int(bottomLeft[1]))
            topLeft     = (int(topLeft[0]), int(topLeft[1]))

            IDtext = f'ID: {markerID}'

            # Display on the image
            cv2.putText(color_image, IDtext,(topLeft[0], topLeft[1] - 15), font, fontScale, fontColor, fontThickness)
    # --------------------- ^ Display ID of marker, by pyImageSearch ^ ---------------------
            
    # Display image
    cv2.imshow('LiveReading', color_image)

    # Store key pressed during 1 [ms] delay
    keyPressed = cv2.waitKey(1)

    # Check for stored key
    if keyPressed == ord('s'):      # S to save frame
        cv2.imwrite(filename=f"screenshot_{num_items}.jpg", img=color_image) # Incrementing filename
        print('Screenshot successful!')
    elif keyPressed == ord('q'):    # Q to stop stream
        break

pipe.stop()             # Stop recording
cv2.destroyAllWindows() # Free resources 