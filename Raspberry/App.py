#--------//====================================//--------#
# A duplex comunication between Arduino and Raspberry py #
# Autor: Alvaro Gabriel Gonzalez Martinez                #
# Version 2.0 - Arduino 25/5/2021                        #
#--------//====================================//--------#

import serial
if __name__ == '__main__':
    # In my case, i read the Arduino by using 'ls /dev/tty*'
    # then i found my Arduino NANO at the 'ttyUSB0'
    ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)
    ser.flush()
    while True:
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').rstrip()
            print(line)