make sure that $ARDMK_DIR is set to point to where the Arduino.mk file is

run: bash INSTALL.sh

if this works, then run the avrdude script

run: bash UPLOAD.sh

This version has working upload to influxdb without arduino Strings, but no working menus due to a memory leak
