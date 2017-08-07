#!/bin/bash

PORT='/dev/tty.usbserial-FTG6PM4N'

if [ $# -ge 1 ]; then
	PORT=$1
fi

#./stm32loader.py -p $PORT -w -e -v images/radio.bin
stm32flash -w images/radio.bin -b 115200 $PORT

