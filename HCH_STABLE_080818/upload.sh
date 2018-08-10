#!/bin/bash

cd source/build-pro5v328-avr

avrdude -v -p m328p -c arduino -P /dev/ttyUSB0 -b 57600 -D -U flash:w:source.hex:i