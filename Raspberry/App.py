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
    # We start the loop
    while True:
        try:
            # Read the line in case it is available the Serial port
            if ser.in_waiting > 0:
                line = ser.readline().decode('latin-1').rstrip()
                print(line)
            # Read a lineat the keyboard
            Message = input("Message: ")
            Message.encode('latin-1')
            ser.write(Message)
        except KeyboardInterrupt:
            break