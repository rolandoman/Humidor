make sure that $ARDMK_DIR is set to point to where the Arduino.mk file is

run: bash INSTALL.sh

if this works, then run the avrdude script

run: bash UPLOAD.sh

This version expands on 100818 and fixes the menu problem due to the memory leak. Required a stop commmand inside comms function (without a flush command which takes too long)
