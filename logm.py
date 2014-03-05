#! /bin/python

import serial
import sys

PORT="/dev/tty.usbserial-FTG6PM4N"
try:
    s = serial.Serial(
        port=PORT,
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
    o = open('data.log', 'w')
    while 1:
        line = s.readline()
        if "$" in line:
            o.write(line)
        o.flush()

        sys.stdout.write(line)
        sys.stdout.flush()

except KeyboardInterrupt:
    print 'Close serial'
    s.flush()
    s.close()
    o.close()
