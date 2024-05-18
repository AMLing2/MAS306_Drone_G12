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
        
    def hall_callback(self, channel):
        '''
        For retriving number of passes from the hallsensor
        '''
        if channel == self.C1:
            self.C1_count += 1
        if channel == self.C2:
            self.C2_count += 1        
        
    def pwm_stop(self):
        self.stop()  #Stop all movement
    
    def PID(self, in_val, last_val, k_list):
        '''
        This function return the values needed
        for calcualting th PID tuning mekanism.
        -Økter
        '''
        dt = time.time() - self.last_time
        p_val = k_list[0] * in_val
        i_mel = self.i_last + ((last_val + in_val)/2) * dt
        i_val = i_mel * k_list[1]
        d_val = ((in_val - last_val)/dt) * k_list[2]
        
        val = p_val + i_val + d_val
        self.i_last = i_mel
        print(dt)
        self.last_time = time.time()
        return val, dt
    
    def check_hall(self):
        C1_now_count = self.C1_count
        C2_now_count = self.C2_count
        
        self.C1_count = 0
        self.C2_count = 0
        
        return C1_now_count, C2_now_count
        
    def go_to_pos(self, new_ang):
        '''
        Main control of the motor controling the positin based on
        feedbak from the hallsensor
        '''
        global y_list
        global x_list
        error = new_ang - self.curr_ang
        
        #if 0.00001 > error > -0.00001:
        #   error = 0
        
        if error > 0:
            if self.dir == 1:
                self.motor.stop()
                self.dir = 0
            GPIO.output(self.AIN1, GPIO.HIGH)
            GPIO.output(self.AIN2, GPIO.LOW)
        elif error < 0:
            if self.dir == 0:
                self.motor.stop()
                self.dir = 1
            GPIO.output(self.AIN1, GPIO.LOW)
            GPIO.output(self.AIN2, GPIO.HIGH)
        else:
            GPIO.output(self.AIN1, GPIO.LOW)
            GPIO.output(self.AIN2, GPIO.LOW)
            
        new_speed, new_dt = self.PID(error, self.last_error, [20,10,0])
        new_speed = abs(new_speed)
        if new_speed > 100:
            new_speed = 100
        elif new_speed < 0:
            new_speed = 0
        self.motor.ChangeDutyCycle(new_speed)
        
        y_list.append(self.curr_ang)
        x_list.append(x_list[-1] + new_dt)
        
        self.last_error = error
        C1_now, C2_now = self.check_hall()
        
        if error > 0:
            self.curr_ang += 0.5 *(C1_now/self.hall_per_round) + 0.5*(C2_now/self.hall_per_round)
        elif error < 0:
            self.curr_ang -= 0.5*(C1_now/self.hall_per_round) + 0.5*(C2_now/self.hall_per_round)
        
        #print(time.time())

    def runPWM(self):
        GPIO.output(self.AIN1, GPIO.HIGH)
        GPIO.output(self.AIN2, GPIO.LOW)
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




