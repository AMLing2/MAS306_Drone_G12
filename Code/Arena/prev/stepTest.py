import RPi.GPIO as GPIO
from RpiMotorLib import RpiMotorLib
from time import sleep

#Setup
direc = 22
step = 23
EN_pin = 24

GPIO.setmode(GPIO.BCM)

#GPIO.setup(direc, GPIO.OUT)
#GPIO.setup(step, GPIO.OUT)

#for x in range(50):
#    GPIO.output(step, GPIO.HIGH)
#    sleep(0.02)
#    GPIO.output(step, GPIO.LOW)


#    sleep(0.02)

stepMotor = RpiMotorLib.A4988Nema(direc, step, (21,21,21), "DRV8825")
GPIO.setup(EN_pin, GPIO.OUT)

#Motor Control
GPIO.output(EN_pin, GPIO.LOW)
stepMotor.motor_go(False, "Full", 200, 0.0005, False, 0.05)

GPIO.cleanup()







