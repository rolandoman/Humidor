/*          SETUP FUNCTION          */
void setup() {

  hchID = 15; //comment this out for writing initial values only - run once
  updateEEPROM(); //comment this out for writing initial values only - run once

  Ethernet.init(53);  // Added for working with Mega 2560

  // start the serial port... turn off in production mode - way too wasteful with memory!
  Serial.begin(57600);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }



  lcd.begin();  // start LCD Screen
  lcdSetup(); // print the initial screen
  ioSetup();  // setup which arduino pins are INPUT, and OUTPUT...

  delay(250); // give the ethernet module time to boot up: This was soooo crucial!!!


  // start the Ethernet connection:
Serial.println("Initialize Ethernet with DHCP:");
if (Ethernet.begin(mac) == 0) {
  Serial.println("Failed to configure Ethernet using DHCP");
  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }
  // try to configure using IP address instead of DHCP:
  //Ethernet.begin(mac, ip, dnServer);
} else {
  Serial.print("  DHCP assigned IP ");
  Serial.println(Ethernet.localIP());
}
// give the Ethernet shield a second to initialize:
delay(1000);


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

  watchdogSetup();  // make sure that watchdog will reboot if prog hangs

} // end setup
