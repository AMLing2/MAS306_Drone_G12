import signal
import sys
import RPi.GPIO as GPIO
from time import sleep
import matplotlib.pyplot as plt

C1 = 25
C2 = 16

C1_count = 0
C2_count = 0

GPIO.setmode(GPIO.BCM)
GPIO.setup(C1, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(C2, GPIO.IN, pull_up_down=GPIO.PUD_UP)

x_list = [0]
c1_list = [0]
c2_list = [2]

def signal_handler(sig, frame):
    global C1_count
    global C2_count
    print("C1 count: ", C1_count, "   C2 count: ", C2_count)
    GPIO.cleanup()
    sys.exit(0)

def button_pressed_callback(channel):
    global C1_count
    global C2_count
    if channel == C1:
        C1_count += 1
        c1_list[-1] = 1
        #print("Pin ", channel," count is: ", C1_count)
    elif channel == C2:
        C2_count += 1
        c2_list[-1] = 3
        #print("Pin ", channel," count is: ", C2_count)

def wait_for_edge():
    CW_count = 0
    edge = 4
    for i in range(10):
        if GPIO.input(C1) == GPIO.HIGH:
            if GPIO.input(C2) == GPIO.LOW:
                CW_count += 1
        sleep(0.01)
    if CW_count > edge:
        print("CW -" + str(CW_count))
    elif CW_count <= edge:
        print("CCW -" + str(CW_count))
            
        
#print("Initsialiserer")
#GPIO.wait_for_edge(C2, GPIO.BOTH)
print("Start")

GPIO.add_event_detect(C1, GPIO.RISING, callback=button_pressed_callback)
GPIO.add_event_detect(C2, GPIO.RISING, callback=button_pressed_callback)

while True:
    #x_list.append(x_list[-1] + 1)
    #c1_list.append(0)
    #2_list.append(2)
    #if len(x_list) > 1000: break
    wait_for_edge()
    sleep(0.1)
    

fig, ax = plt.subplots()
ax.grid()
ax.set_xlabel("Iteration")
ax.set_ylabel("Angle")
ax.plot(x_list, c1_list)
ax.plot(x_list, c2_list)
plt.show()

print(c1_list)

#signal.signal(signal.SIGINT, signal_handler)
#signal.pause()


