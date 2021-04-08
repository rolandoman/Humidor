#!/bin/bash

cd source/build-mega2560-avr

/snap/arduino/50/hardware/tools/avr/bin/avrdude -C/snap/arduino/50/hardware/tools/avr/etc/avrdude.conf -v -patmega2560 -c wiring -P /dev/ttyUSB0 -b 115200 -D -U flash:w:source.hex:i
