import cv2                  # OpenCV
import pyrealsense2 as rs   # RealSense Wrapper
import numpy                # Python Math
import os                   # For path
import multiprocessing as mp
#qualisys imports:
import asyncio
import xml.etree.ElementTree as ET
import pkg_resources

import qtm


# -------------------Qualisys setup -------------------
def create_body_index(xml_string):
    """ Extract a name to index dictionary from 6dof settings xml """
    xml = ET.fromstring(xml_string)

    body_to_index = {}
    for index, body in enumerate(xml.findall("*/Body/Name")):
        body_to_index[body.text.strip()] = index

    return body_to_index

async def main(qTime):

    # Connect to qtm ----------------------------------------------------------
    connection = await qtm.connect("192.168.0.29")

    # Connection failed?
    if connection is None:
        print("Failed to connect")
        return


    # Get 6dof settings from qtm
    xml_string = await connection.get_parameters(parameters=["6d"])
    body_index = create_body_index(xml_string)


    # Define which body to extract ----------------------------------------------
    wanted_body = "drone"

    def on_packet(packet):
        info, bodies = packet.get_6d()
        

        if wanted_body is not None and wanted_body in body_index:
            # Extract one specific body
            wanted_index = body_index[wanted_body]
            position, rotation = bodies[wanted_index]
            x, y, z = position
            allRotations = rotation
            rotation1 = rotation[0][0]




            #print("{} - Pos: {} - Rot: {}".format(wanted_body, position, rotation))
            #print(f"X: {x}, Y: {y}, Z: {z}")
            #print(f"Rotation matrix: {allRotations}")
            print(f"Matrix: {rotation1}")
            
        
    # Start streaming frames
    await connection.stream_frames(components=["6d"], on_packet=on_packet)

    # Wait asynchronously 5 seconds
    await asyncio.sleep(5000)

    # Stop streaming
    await connection.stream_frames_stop()

# -------------------Qualisys setup --------------------
def cameraMethod(pipe,qTime,recording):
	while(True):
			# Extract and convert frame
			frame = pipe.wait_for_frames() # waits for and collects all frames from camera (depth, color, etc)
			color_frame = frame.get_color_frame()
			color_image = numpy.asanyarray(color_frame.get_data())

			# Save frame to file
			if not qTime.empty():
					recording.write(color_image)
					a = qTime.get(block=False)

			# Display the current frame
			cv2.imshow('LiveReading', color_image)

			# Press Q to stop recording
			if cv2.waitKey(1) == ord('q'):
					break
	print("camera process done")

if __name__ == "__main__":
	
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

# Setup Stream
	cfg.enable_stream(rs.stream.color, w, h, rs.format.bgr8, fps) # (streamType, xRes, yRes, format, fps)
	pipe.start(cfg)

	qCam = mp.Queue(5)
	qQualisys = asyncio.Queue(5)
	pCam = mp.Process(target=cameraMethod, args=(pipe,qCam,recording,))
	# Run our asynchronous function until complete
	pCam.start()
	asyncio.get_event_loop().run_until_complete(main(qQualisys))

	pCam.join()
	recording.release()
	pipe.stop()             # Stop recording
	cv2.destroyAllWindows() # Free resources


