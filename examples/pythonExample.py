import serial
import time
from serial.tools import list_ports

# TODO: change usbmodem45 to whatever your system shows when you enter
coms = serial.tools.list_ports.comports()
likelyPort = coms[['usbmodem' in x[0] for x in coms].index(True)][0]

print "Trying port ", likelyPort

ser = serial.Serial(likelyPort, 9600, timeout=1)#or something like this for Mac/Linux machines

t1 = []
t2 = []
amt = 10
for x in range(0,amt):
    t1.append(time.clock())
    ser.write('T')
    line = ser.readline()   # read a '\n' terminated line
    t2.append(time.clock())

print "30x t1: "
print t1
print "30x t2: "
print t2
print (sum(t2) - sum(t1))

ser.close()

