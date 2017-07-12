import RPi.GPIO as GPIO           # import RPi.GPIO module  
import picamera 
import time 
import wiringpi
import ivport
import threading
from PIL import Image

GPIO.setmode(GPIO.BCM)            # choose BCM or BOARD  
GPIO.setup(pin, GPIO.IN)  		  # set a port/pin as an input  

# use 'GPIO naming'
wiringpi.wiringPiSetupGpio()
# set #18 to be a PWM output
wiringpi.pinMode(18, wiringpi.GPIO.PWM_OUTPUT)
# set the PWM mode to milliseconds stype
wiringpi.pwmSetMode(wiringpi.GPIO.PWM_MODE_MS)
# divide down clock
wiringpi.pwmSetClock(192)
wiringpi.pwmSetRange(2000)
#Ultrasonic sensor setup
TRIG1 = 23	#Pin number of Trig1
ECHO1 = 24	#Pin number of echo1
GPIO.setup(TRIG1,GPIO.OUT)
GPIO.setup(ECHO1,GPIO.IN)
TRIG2 = 23	#Pin number of Trig2
ECHO2 = 24	#Pin number of echo2
GPIO.setup(TRIG2,GPIO.OUT)
GPIO.setup(ECHO2,GPIO.IN)
TRIG3 = 23	#Pin number of Trig3
ECHO3 = 24	#Pin number of echo3
GPIO.setup(TRIG3,GPIO.OUT)
GPIO.setup(ECHO3,GPIO.IN)
TRIG3 = 23	#Pin number of Trig4
ECHO3 = 24	#Pin number of echo4
GPIO.setup(TRIG3,GPIO.OUT)
GPIO.setup(ECHO3,GPIO.IN)
GPIO.output(TRIG1, False)
print "Waiting For Sensor1 To Settle"
time.sleep(2)
GPIO.output(TRIG2, False)
print "Waiting For Sensor2 To Settle"
time.sleep(2)
GPIO.output(TRIG3, False)
print "Waiting For Sensor3 To Settle"
time.sleep(2)
GPIO.output(TRIG4, False)
print "Waiting For Sensor4 To Settle"
time.sleep(2)

#Camera multiplexer setup
iv = ivport.IVPort(ivport.TYPE_DUAL2)

camera = picamera.PiCamera()

cur_camera = 1 #1 is IR 2 is Visible light
talons = 0	#0 is open 1 is closed

def getTemperature(pin):

	temp = GPIO.input(pin)
	camera.annotate_text = temp
	return temp
	
def openTalons():
	
	for pulse in range(50, 250, 1):
        wiringpi.pwmWrite(18, pulse)

def closeTalons():
	for pulse in range(250, 50, -1):
        wiringpi.pwmWrite(18, pulse)

def recieveCommands():
	return command

def getUltrasonic(sensor):
	if sensor == 1:
		TRIG = TRIG1
		ECHO = ECHO1
	elif sensor == 2:
		TRIG = TRIG2
		ECHO = ECHO2
	elif sensor == 3:
		TRIG = TRIG3
		ECHO = ECHO3
	elif sensor == 4:
		TRIG = TRIG4
		ECHO = ECHO4

	GPIO.output(TRIG, True)
	time.sleep(0.00001)
	GPIO.output(TRIG, False)
	while GPIO.input(ECHO)==0:
		pulse_start = time.time()
	while GPIO.input(ECHO)==1:
		pulse_end = time.time()      
	pulse_duration = pulse_end - pulse_start
	distance = pulse_duration x 17150
	distance = round(distance, 2)
	print "Distance:",distance,"cm"
	return distance

class tempThread (threading.Thread):
    def __init__(self, threadID, name,pin):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.name = name
        self.pin = pin
    def run(self):
        print "Starting " + self.name
        
        while True:
        	temp = GPIO.input(self.pin)
			camera.annotate_text = temp
			for i in range(img.size[1]):    # for every col:
    			for j in range(img.size[0]):    # For every row
        			pixels[i,j] = (temp, altitude, UltraSonic) # set the colour accordingly
        	
        print "Exiting " + self.name

class usThread (threading.Thread):
    def __init__(self, threadID, name, sensor):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.name = name
        self.sensor = sensor
    def run(self):
        print "Starting " + self.name
        while True:
        	if self.sensor == 1:
        		distance1 = getUltrasonic(1)	# Get ultrasonic distance 1
        	elif self.sensor == 2:
        		distance2 = getUltrasonic(2)	# Get ultrasonic distance 2
        	elif self.sensor == 3:
        		distance3 = getUltrasonic(3)	# Get ultrasonic distance 3
        	elif self.sensor == 4:
        		distance4 = getUltrasonic(4)	# Get ultrasonic distance 4
        print "Exiting " + self.name
try:  
	command = 0
	img = Image.new( 'RGB', (100,250), "black") # create a new black image
	pixels = img.load() # create the pixel map
	# Create new threads
	tempthread1 = tempThread(1, "Thread-Temperature", pin)	#replace pin with tempsensor gpio pin
	usthread1 = usThread(2, "Thread-Ultrasonic Sensor 1", 1)
	usthread2 = usThread(3, "Thread-Ultrasonic Sensor 2", 2)
	usthread3 = usThread(4, "Thread-Ultrasonic Sensor 3", 3)
	usthread4 = usThread(5, "Thread-Ultrasonic Sensor 4", 4)
	# Start new Threads
	tempthread1.start()
	usthread1.start()
	usthread2.start()
	usthread3.start()
	usthread4.start()
	camera.start_preview()
	img.show()
    while True:            # this will carry on until you hit CTRL+C  
        
        command = recieveCommands()   	# Get commands

        if command == 0:
   			print "No new command"
        elif command == 1:		#switch camera
   			if cur_camera == 1: #IR camera
   				iv.camera_change(2)
   				print "Now using Visible light camera"
   			if cur_camera == 2: #Visible light camera
   				iv.camera_change(1)
   				print "Now using IR camera"
		elif command == 2:		#open/close talons
		    if talons == 0: 	#talons are open so close
   				closeTalons()
   				print "talons are now closed"
   			if talons == 1: 	#talons are closed so open
   				openTalons()
   				print "talons are now open"
		elif command == 3:
		   
		else:
		   print "Error recieving Commands"
  
finally:                   
    GPIO.cleanup()   
    iv.close()