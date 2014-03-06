#! /bin/python

import serial
import sys
import datetime
import os


PORT="/dev/ttyUSB0"
if len(sys.argv) > 1:
    PORT=sys.argv[1]

print "Open ", PORT

try:
    s = serial.Serial(
        port=PORT,
        baudrate=115200,         # baudrate
        bytesize=8,             # number of databits
        parity=serial.PARITY_NONE,
        stopbits=1,
        xonxoff=0,              # enable software flow control
        rtscts=0,               # disable RTS/CTS flow control
        timeout=5               # set a timeout value, None for waiting forever
        )

    s.setDTR(0) #disabilito reset
    s.setRTS(1) #disabilito boot0

    curr_file_name = ""
    o = None
    while 1:
        file_name = datetime.datetime.today().strftime("%Y%m%d")
        if curr_file_name != file_name:
            curr_file_name = file_name
            o = open(os.path.join("log", curr_file_name + ".log"), 'w+')

        line = s.readline()
        line = line.strip()
        if line:
            date = datetime.datetime.today().strftime("%d/%m/%Y")
            time = datetime.datetime.today().strftime("%X")

            if "$" in line and o is not None:
                line = line.replace("$","")
                line = line.strip()
                line = "%s;%s;%s;%s\n" % (line[0], date, time, line[1:])
                o.write(line)
            o.flush()

            sys.stdout.write(line)
            sys.stdout.flush()

except KeyboardInterrupt:
    print 'Close serial'
    s.flush()
    s.close()
    o.close()
