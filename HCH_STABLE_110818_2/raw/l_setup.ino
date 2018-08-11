/*          SETUP FUNCTION          */
void setup() {
  // start the serial port... turn off in production mode - way to wasteful with memory!
  //Serial.begin(57600);

  lcd.begin();  // start LCD Screen
  lcdSetup(); // print the initial screen
  ioSetup();  // setup which arduino pins are INPUT, and OUTPUT...
  delay(250); // give the ethernet module time to boot up: This was soooo crucial!!!
  Ethernet.begin(mac, ip, dnServer, gateway, subnet);
  eclient.setTimeout(3000);
  delay(500);

  //udp.begin(udpport);
  // setup the temp and humidity sensors
  dht1.begin();dht2.begin();dht3.begin();

  // all the time intervals are set in aa_definitions file
  //timer.setInterval(uploadInterval, sendData);
  //timer.setInterval(measureInterval, readSensorData);
  timer.setInterval(updateLCDInterval, updateLCD);
  timer.setInterval(updateDaytimeInterval, isDaytime);

  watchdogSetup();  // make sure that watchdog will reboot if prog hangs

} // end setup
