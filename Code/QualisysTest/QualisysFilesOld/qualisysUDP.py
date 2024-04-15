"""
    Streaming 6Dof from QTM
"""

import asyncio
import xml.etree.ElementTree as ET
import pkg_resources

import qtm

def create_body_index(xml_string):
    """ Extract a name to index dictionary from 6dof settings xml """
    xml = ET.fromstring(xml_string)

    body_to_index = {}
    for index, body in enumerate(xml.findall("*/Body/Name")):
        body_to_index[body.text.strip()] = index

    return body_to_index


async def main():

    # Connect to qtm ---------------------------------------------------------------------
    connection = await qtm.connect("192.168.0.29")

    # Connection failed?
    if connection is None:
        print("Failed to connect")
        return


    # Get 6dof settings from qtm
    xml_string = await connection.get_parameters(parameters=["6d"])
    body_index = create_body_index(xml_string)


    # Define which body to extract ------------------------------------------------------
    wanted_body = "tooltip"

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
    await asyncio.sleep(1000)

    # Stop streaming
    await connection.stream_frames_stop()


if __name__ == "__main__":
    # Run our asynchronous function until complete
    asyncio.get_event_loop().run_until_complete(main())
