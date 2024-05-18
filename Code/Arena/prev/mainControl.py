#Martin Økter
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

#Class setup
class stepper_motor:
    def __init__(self, direc, step, EN_pin, mode, base_wait_time, stepPerRot, roundUpDown):
        self.direc_pin = direc
        self.step_pin = step
				#self.EN_pin = EN_pin
        self.mode = mode
        self.base_wait_time = base_wait_time
        self.stepPerRot = stepPerRot
        self.roundUpDown = roundUpDown
        
        self.resolution = {'Full':(0,0,0), 'Half':(1,0,0), '1/4':(0,1,0), '1/8':(1,1,0), '1/16':(0,0,1), '1/32':(1,0,1)}
        self.resolution_multiplier = {'Full':1, 'Half':2, '1/4':4, '1/8':8, '1/16':16, '1/32':32}
        self.resolution_choice = '1/16' #Best movement for the stepper in use
        
        GPIO.setup(self.direc_pin, GPIO.OUT)
        GPIO.setup(self.step_pin, GPIO.OUT)
        #GPIO.setup(self.EN_pin, GPIO.OUT)
        GPIO.setup(self.mode, GPIO.OUT)
        
        #Stadard pins set
				#GPIO.output(EN_pin, GPIO.LOW)
        for i in range(3):
            GPIO.output(self.mode, self.resolution[self.resolution_choice]) 
        
    def go_rounds(self, num_of_rounds, direction):
        '''
        Make the stepper go compete rounds for basic manuvering
        with more easy to use number
        May handle float number
        '''
        #Determine direction
        if direction == 'Up':
            GPIO.output(self.direc_pin, GPIO.LOW)
        elif direction == 'Down':
            GPIO.output(self.direc_pin, GPIO.HIGH)
        
        mode_multi = self.resolution_multiplier[self.resolution_choice]
        adjuster = 10
        
        #Calculate number of steps
        number_of_steps = round(num_of_rounds*self.stepPerRot*mode_multi)
        
        #Makes steps
        for x in range(number_of_steps):
                GPIO.output(self.step_pin, GPIO.HIGH)
                time.sleep(self.base_wait_time/(mode_multi*adjuster))
                GPIO.output(self.step_pin, GPIO.LOW)
                time.sleep(self.base_wait_time/(mode_multi*adjuster))
        
    def go_up(self):
        self.go_rounds(roundUpDown,'Up') #Make the piston move up max

    def go_down(self):
        self.go_rounds(roundUpDown,'Down') #Make the piston move down max
    
    def set_resolution(self, new_resolution):
        '''
        Set a new reutution of the stepper adjusting between microstepping and full
        '''
        self.resolution_choice = new_resolution
        for i in range(3):
            GPIO.output(self.mode, self.resolution[self.resolution_choice])
        

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
mainStepper = stepper_motor(22,23,24,(8,7,1),0.0416,200,2.8)
mainRotMot = pwm_motor(12,2000,25,16,27,17)

#mainStepper.go_rounds(1,'Down','Up',1234567890)
while True:
    print('Initiate run')
    mainStepper.set_resolution('1/16')
    mainStepper.go_rounds(0.3,'Down')
    print('Run compleat')
    #mainRotMot.go_to_pos(1)
    #signal.signal(signal.SIGINT, signal_handler)
    #signal.pause()
    time.sleep(10)




