# From tutorials on youtube: 
    # Simple Face Detection in Python \\ https://www.youtube.com/watch?v=5cg_yggtkso
    # OpenCV Python Tutorial For Beginners 4 \\ https://www.youtube.com/watch?v=-RtVZsCvXAQ&t=871s
    # GeeksForGeeks for saving video \\ https://www.geeksforgeeks.org/saving-a-video-using-opencv/

import pathlib # To access existing xml files for facial recognition
import cv2

# Finding the xml file location
cascade_path = pathlib.Path(cv2.__file__).parent.absolute() / "data/haarcascade_frontalface_default.xml"
#print(cascade_path)
classifier = cv2.CascadeClassifier(str(cascade_path))

camera = cv2.VideoCapture(0)

# Recording done through GeeksForGeeks
frame_width = int(camera.get(3)) 
frame_height = int(camera.get(4))    
fourCharCode = cv2.VideoWriter_fourcc(*'MJPG')
output = cv2.VideoWriter('FacialRecognitionThomas.avi', fourCharCode, 30, (frame_width, frame_height))

while(True):
    returnValue, frame = camera.read()  # Live reading from laptop camera #0

    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    # Library uses Haar Cascade Filter: https://docs.opencv.org/3.4/db/d28/tutorial_cascade_classifier.html
    faces = classifier.detectMultiScale(
        gray,
        scaleFactor=1.1,
        minNeighbors=5,
        minSize=(30,30),
        flags=cv2.CASCADE_SCALE_IMAGE
    )

    for (x, y, width, height) in faces:
        cv2.rectangle(frame, (x, y), (x+width, y+height), (255, 255, 0), 2)


    cv2.imshow('Faces', frame)     # Display the current frame
    output.write(frame)


    if cv2.waitKey(1) == ord('q'): # user input (after 1 ms) == q
        break

camera.release()
cv2.destroyAllWindows()