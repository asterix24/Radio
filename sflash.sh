PORT="/dev/tty.PL2303-0000101D"

if [ $# -ge 1 ]; then
	PORT=$1
fi

./stm32loader.py -p $PORT -w -e -v images/radio.bin
