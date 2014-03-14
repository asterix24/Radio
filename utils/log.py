#! /bin/python

import serial
import serial.tools.list_ports
import sys
import datetime
import os


port = ""
if len(sys.argv) > 1:
    port=sys.argv[1]

if '-l' in port:
    for i in serial.tools.list_ports.comports():
        print "%10s %20s %s\n" % (i[0], i[1], i[2]),
    sys.exit(0)

log_dir = "/tmp"
if len(sys.argv) > 2:
    log_dir=sys.argv[2]

PORT="/dev/ttyUSB0"
if port:
    for i in serial.tools.list_ports.comports():
        if port in i[1]:
            PORT = i[0]
            break

print "Open: ", PORT

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
            file = os.path.join(log_dir, curr_file_name + ".log")
            o = open(file, 'w+')
            print "Log file: ", file

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
