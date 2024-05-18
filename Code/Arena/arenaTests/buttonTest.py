import RPi.GPIO as GPIO
from RpiMotorLib import RpiMotorLib
from time import sleep

#Setup
button = 6

direc = 22
step = 23
#EN_pin = 24
mode = (8,7,1)

GPIO.setmode(GPIO.BCM)

GPIO.setup(button, GPIO.IN, pull_up_down=GPIO.PUD_UP)
#GPIO.setup(direc, GPIO.OUT)
#GPIO.setup(step, GPIO.OUT)
#GPIO.setup(EN_pin, GPIO.OUT)
#GPIO.setup(mode, GPIO.OUT)

#GPIO.output(EN_pin, GPIO.LOW)
#GPIO.output(direc, GPIO.HIGH)
#for i in range(3):
#    GPIO.output(mode, resolution['1/32'])

#numOfRot = 2
#modeNum = 32

#for x in range(numOfRot*stepPerRot*modeNum):
#    GPIO.output(step, GPIO.HIGH)
#    sleep(waitTime/(modeNum*10))
#    GPIO.output(step, GPIO.LOW)
#    sleep(waitTime/(modeNum*10))

#stepMotor = RpiMotorLib.A4988Nema(direc, step, (21,21,21), "DRV8825")

btnPressed = True
i = 0
print("waiting for button press")

while(btnPressed):
	if not GPIO.input(button):
		print("button pressed: " + str(i))
		i += 1
		sleep(0.5)
	if i >=5:
		btnPressed = False

#Motor Control

# stepMotor.motor_go(False, "Full", 200, 0.0005, False, 0.05)

GPIO.cleanup()

