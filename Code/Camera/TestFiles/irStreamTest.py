import pyrealsense2 as rs
import numpy as np
import cv2

# Create a configuration for configuring the pipeline with a non-default profile
cfg = rs.config()

# Add desired streams to configuration
cfg.enable_stream(rs.stream.infrared, 640, 480, rs.format.y8, 30)

# Instruct pipeline to start streaming with the requested configuration
pipe = rs.pipeline()
pipe.start(cfg)

# Camera warmup - dropping several first frames to let auto-exposure stabilize
for i in range(30):
    # Wait for all configured streams to produce a frame
    frames = pipe.wait_for_frames()

# Get infrared frame
ir_frame = frames.get_infrared_frame()

# Convert the infrared frame to a numpy array
ir_image = np.asanyarray(ir_frame.get_data())

# Write the IR image to a file
cv2.imwrite("ir_image.png", ir_image)

# Close the pipeline
pipe.stop()