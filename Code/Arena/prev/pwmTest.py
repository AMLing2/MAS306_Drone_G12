import RPi.GPIO as GPIO
from time import sleep


rotPin = 12
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
GPIO.setup(rotPin, GPIO.OUT)
rot_pwm = GPIO.PWM(rotPin, 2000)

rot_pwm.start(0)

sleep(1)

rot_pwm.ChangeDutyCycle(20)

sleep(1)

rot_pwm.stop()
GPIO.cleanup()
