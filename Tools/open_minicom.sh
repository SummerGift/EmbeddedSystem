# setserial -v /dev/ttyUSB0 spd_cust divisor $((24000000/250000))
minicom -D /dev/ttyUSB0 -b 115200
