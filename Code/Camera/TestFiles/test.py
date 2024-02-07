# ProgrammingKnowledge Youtube Tutorial

import cv2

img = cv2.imread('C:/Users/Thoma/OneDrive - Universitetet i Agder/UiA/6. semester/MAS306/MAS306_Drone_G12/Code/Camera/lena.jpg', 0)

print(img)

cv2.imshow('Image', img)
cv2.waitKey(5000)
cv2.destroyAllWindows

# Added test line