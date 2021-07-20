/*          SETUP FUNCTION          */
void setup() {

  #if defined(FIRSTTIME) &&  FIRSTTIME
    updateEEPROM(); //comment this out for writing initial values only - run once
    updatePIDvals(); // zero out the PID stored values for initial run
  #else
    readEEPROM(); // get all the stored values
    readPIDvals(); // get PID vals if they were happily stored from last run
  #endif

  setHeater(); // after reading default values, set up heater and LED
  setLED();

  Ethernet.init(53);  // Added for working with Mega 2560

  // start the serial port... turn off in production mode - way too wasteful with memory!
  #if defined(DEBUG) &&  DEBUG
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  #endif


  lcd.begin();  // start LCD Screen
  lcdSetup(); // print the initial screen
  ioSetup();  // setup which arduino pins are INPUT, and OUTPUT...

  delay(250); // give the ethernet module time to boot up: This was soooo crucial!!!


  // start the Ethernet connection:
  #if defined(DEBUG) &&  DEBUG
  Serial.println("Initialize Ethernet with DHCP:");
  #endif
  if (Ethernet.begin(mac) == 0) {
    #if defined(DEBUG) &&  DEBUG
    Serial.println("Failed to configure Ethernet using DHCP");
    #endif
  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    #if defined(DEBUG) &&  DEBUG
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    #endif
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    #if defined(DEBUG) &&  DEBUG
    Serial.println("Ethernet cable is not connected.");
    #endif
  }
  // try to configure using IP address instead of DHCP:
  //Ethernet.begin(mac, ip, dnServer);
} else {
  #if defined(DEBUG) &&  DEBUG
  Serial.print("  DHCP assigned IP ");
  Serial.println(Ethernet.localIP());
  #endif
}
// give the Ethernet shield a second to initialize:
delay(500);


  //Ethernet.begin(mac, ip, dnServer, gateway, subnet);
  //eclient.setTimeout(3000);





  delay(500);

  udp.begin(udpport);
  // setup the temp and humidity sensors
  dht1.begin();dht2.begin();dht3.begin();

  // all the time intervals are set in aa_definitions file
  timer.setInterval(uploadInterval, sendData);
  timer.setInterval(measureInterval, readSensorData);
  timer.setInterval(updateLCDInterval, updateLCD);
  timer.setInterval(updateDaytimeInterval, isDaytime);
  timer.setInterval(updatePIDvalsInterval, updatePIDvals);

  watchdogSetup();  // make sure that watchdog will reboot if prog hangs

} // end setup
