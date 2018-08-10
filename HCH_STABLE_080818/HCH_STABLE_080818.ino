/* 
  
Roland Clarke - July 2018

Test script for verifying buttons on front of HCH controller
Version 2 testing
Build better library control, and divide project into multiple files for easier development

0) this file - top comments, also all the includes for the project (entire project so that libraries are all good for all test files)
1) definitions.h - compiler define statements
2) a_setup.ino - initiate libraries and run setup loop
3) main.ino - main function loop only 
4) watchdog.ino - all the watchdog functions: resetFunc, watchdogSetup, ISR

Should follow this format as more and more functions or tests are added to the project
*/

#include <avr/wdt.h>
#include <SPI.h>
#include <Ethernet.h>
#include <SimpleTimer.h>
#include <DHT.h>
#include "Wire.h"
#include <LiquidCrystal_I2C.h>
#include <Button.h>
#include "aa_definitions.h"

