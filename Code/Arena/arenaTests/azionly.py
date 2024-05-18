#Main controll of mid actuatror in the Drone Arena
# 10/2/2022

import signal
import sys
import RPi.GPIO as GPIO
import time
import matplotlib.pyplot as plt

#Setup GPIO
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

y_list = [0]
x_list = [0]

class pwm_motor:
    def __init__(self, motor_pin, freq, C1_pin, C2_pin, AIN1_pin, AIN2_pin):
        self.C1 = C1_pin
        self.C2 = C2_pin
        self.AIN1 = AIN1_pin
        self.AIN2 = AIN2_pin
        self.C1_count = 0
        self.C2_count = 0
        
        self.hall_per_round = 300
        
        self.curr_ang = 0
        self.last_error = 0
        self.i_last = 0
        
        self.last_time = time.time() #Set a start time as a setpoint
        self.dir = 0
        
        GPIO.setup(motor_pin, GPIO.OUT)
        GPIO.setup(self.AIN1, GPIO.OUT)
        GPIO.setup(self.AIN2, GPIO.OUT)
        
        #Setup hall interupt
#GPIO.setup(self.C1, GPIO.IN, pull_up_down=GPIO.PUD_UP)
#       GPIO.setup(self.C2, GPIO.IN, pull_up_down=GPIO.PUD_UP)
 #	GPIO.add_event_detect(self.C1, GPIO.RISING, callback=self.hall_callback)
#      GPIO.add_event_detect(self.C2, GPIO.RISING, callback=self.hall_callback)
        
        self.motor = GPIO.PWM(motor_pin, freq)
        self.motor.start(0)
        
    def pwm_stop(self):
        self.motor.stop()  #Stop all movement
    

    def runPWM(self):
#        GPIO.output(self.AIN1, GPIO.HIGH)
#        GPIO.output(self.AIN2, GPIO.LOW)

        GPIO.output(self.AIN1, GPIO.LOW)
        GPIO.output(self.AIN2, GPIO.HIGH)


        self.motor.ChangeDutyCycle(30)
        time.sleep(1)

#Function setup
def signal_handler(sig, frame):
    global y_list
    global x_list
    fig, ax = plt.subplots()
    ax.grid()
    ax.set_xlabel("Iteration")
    ax.set_ylabel("Angle")
    ax.plot(x_list, y_list)
    plt.show()
    GPIO.cleanup()
    sys.exit(0)
   
#Initialize motors
#Initial basetime 0.0208
mainRotMot = pwm_motor(12,2000,25,16,27,17)

#mainStepper.go_rounds(1,'Down','Up',1234567890)
while True:
    print('Initiate run')
    mainRotMot.runPWM()
    print('Run compleat')
    mainRotMot.pwm_stop()
    #signal.signal(signal.SIGINT, signal_handler)
    #signal.pause()
    time.sleep(10)




