#! /bin/env python

import serial
import sys

def_port = '/dev/tty.PL2303-0000101D'
def_baud = 115200

try:
    s = serial.Serial(
        port='/dev/tty.usbserial',
        baudrate=115200,     # baudrate
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
