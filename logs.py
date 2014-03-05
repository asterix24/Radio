#! /bin/env python

import serial
import serial.tools.list_ports
import sys

def_baud = 115200
def_port = ""

for i in serial.tools.list_ports.comports():
    if 'PL2303' in i[0]:
        def_port = i[0]
        print "Open: ", def_port
        break

if def_port == "":
    print "No valid port."
    sys.exit(1)

try:
    s = serial.Serial(
        port=def_port,
        baudrate=def_baud,     # baudrate
        bytesize=8,             # number of databits
        parity=serial.PARITY_NONE,
        stopbits=1,
        xonxoff=0,              # enable software flow control
        rtscts=0,               # disable RTS/CTS flow control
        timeout=5               # set a timeout value, None for waiting forever
        )

    s.setDTR(0) #disabilito reset
    s.setRTS(1) #disabilito boot0
    while 1:
        sys.stdout.write(s.read())
except KeyboardInterrupt:
    print 'Close serial'
    s.flush()
    s.close()
