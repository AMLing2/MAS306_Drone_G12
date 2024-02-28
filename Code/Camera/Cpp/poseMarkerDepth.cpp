
#include <librealsense2/rs.hpp> // Import RealSense Library
// #include <librealsense2/h/rs_pipeline.h>
// #include <librealsense2/h
#include <iostream>             // Standard IO

#include <opencv2/opencv.hpp>

namespace stream {
    int width = 480;
    int height = 848;
    int fps = 60;
}

rs2::pipeline pipe;
rs2::config config;

void setup(){

    config.enable_stream(rs2_stream::RS2_STREAM_DEPTH, stream::width, stream::height, RS2_FORMAT_Z16, stream::fps);

    auto pipeline = pipe.start(config);

    pipeline.get_stream(RS2_STREAM_DEPTH);
}

int main() {

    setup();
    int i = 0;

    while(i < 1000) {
        // Receive frames
        rs2::frameset frames = pipe.wait_for_frames();

        // Extract Depth Frame
        rs2::depth_frame depthFrame = frames.get_depth_frame();

        // Query the distance from the camera to the object in the center of the image
        float dist_to_center = depthFrame.get_distance(stream::width / 2, stream::height / 2);

        std::cout << "\nDistance: " << std::endl;

        i++;
    }

    return 0;
}
