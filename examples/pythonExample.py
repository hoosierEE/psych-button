import time
import serial
from serial.tools import list_ports

# Something funky going on here. 
# Sometimes it works.  Other times it leaves behind the previous
# `likelyPort` and attempts to open it again after it's been closed.
coms = list_ports.comports()
likelyPort = coms[['usbmodem' in x[0] for x in coms].index(True)][0]
print "Trying port ", likelyPort

# open the chosen serial port
ser = serial.Serial(likelyPort, 9600, timeout=1) #or something like this for Mac/Linux machines

# Timing test
t1 = []
t2 = []
amt = 100 
for x in range(0,amt):
    t1.append(time.clock())
    ser.write('T')
    line = ser.readline()   # read a '\n' terminated line
    t2.append(time.clock())

print "t1 times: ", t1
print "t2 times: ", t2
print "average time: ", (sum(t2) - sum(t1)), " seconds"

ser.close()

