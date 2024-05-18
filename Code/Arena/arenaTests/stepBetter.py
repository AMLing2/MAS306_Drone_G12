import RPi.GPIO as GPIO
from RpiMotorLib import RpiMotorLib
from time import sleep

#Setup
direc = 22
step = 23
#EN_pin = 24
mode = (8,7,1)

resolution = {'Full':(0,0,0), 'Half':(1,0,0), '1/4':(0,1,0), '1/8':(1,1,0), '1/16':(0,0,1), '1/32':(1,0,1)}

waitTime = 0.0208
stepPerRot = 200

GPIO.setmode(GPIO.BCM)
GPIO.setup(direc, GPIO.OUT)
GPIO.setup(step, GPIO.OUT)
#GPIO.setup(EN_pin, GPIO.OUT)
GPIO.setup(mode, GPIO.OUT)

#GPIO.output(EN_pin, GPIO.LOW)
GPIO.output(direc, GPIO.HIGH)
for i in range(3):
    GPIO.output(mode, resolution['1/32'])

numOfRot = 2
modeNum = 32

for x in range(numOfRot*stepPerRot*modeNum):
    GPIO.output(step, GPIO.HIGH)
    sleep(waitTime/(modeNum*10))
    GPIO.output(step, GPIO.LOW)
    sleep(waitTime/(modeNum*10))

#stepMotor = RpiMotorLib.A4988Nema(direc, step, (21,21,21), "DRV8825")



#Motor Control

# stepMotor.motor_go(False, "Full", 200, 0.0005, False, 0.05)

GPIO.cleanup()

