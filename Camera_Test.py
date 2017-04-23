import picamera
import time

camera = picamera.PiCamera()
camera.start_preview()
camera.start_recording('/home/pi/test.h264')
camera.annotate_text = "Test Test Test 99887766554433221100"
time.sleep(10)
camera.stop_recording()