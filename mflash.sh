#!/bin/bash

PORT="/dev/tty.usbserial-FTG6Q56R"

if [ $# -ge 1 ]; then
	PORT=$1
fi

./stm32loader.py -p $PORT -w -e -v images/radio.bin

