
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

Major upgrades for using the Mega 2560 Pro Mini board which has more memory
Adding a compiler compare so the code may work on the old boards...


*/

#include <avr/wdt.h>
#include <SPI.h>
#include <Ethernet.h>
#include <SimpleTimer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include "Wire.h"
#include <LiquidCrystal_I2C.h>
#include <Button.h>
#include <EEPROM.h>

#define Mega2560 1

// comment this out for production mode...
#define DEBUG 1
// set this flag if running for first time to burn defaults into EEPROM
//#define FIRSTTIME 1

// special characters for the LCD display, should create a nice logo
byte N1_c[8] = {B11100,B10111,B11101,B00001,B11101,B10111,B11100,B00000};
byte N2_c[8] = {B00000,B00000,B01110,B11010,B01110,B00000,B00000,B00000};
byte degC_c[8] = {B01000,B10100,B01000,B00011,B00100,B00100,B00011,B00000};
#define net1_char 0
#define net2_char 1
#define degrees_C_char 2

// special relay control words
#define Relay_ON HIGH
#define Relay_OFF LOW

// Define all the arduino pins up front...
#if defined(Mega2560) &&  Mega2560
  #define sensPin1 45 // DHT22 #1
  #define sensPin2 46 // DHT22 #2
  #define sensPin3 44 // DHT22 #3
  #define waterPin 31 // level sensor
#else
  #define sensPin1 A0 // DHT22 #1
  #define sensPin2 A1 // DHT22 #2
  #define sensPin3 A2 // DHT22 #3
  #define waterPin A3 // level sensor
#endif


// Output pins for control
#if defined(Mega2560) &&  Mega2560
  #define waterPumpPin 23
  #define peltierPwmPin 13  // Turns heat/cool module on and off (default cool, pin 6 low - heat)
  #define heatCoolPin 12   // Default is Cool mode, pin low is Heat mode
  #define lightPin 30 // controls relay to led lights
  #define mistPin 25
#else
  #define waterPumpPin 7    // controls power to heat/cool module in auto mode - do not use...
  #define peltierPwmPin 3   // Turns heat/cool module on and off (default cool, pin 6 low - heat)
  #define heatCoolPin 6 // Default is Cool mode, pin low is Heat mode
  #define lightPin 5  // controls relay to led lights
  #define mistPin 9   // controls the humidifier relay
#endif
// input pins
#if defined(Mega2560) &&  Mega2560
  #define topButton 43   // top mode button connected to digital pin 2
  #define midButton 41   // middle increase button connected to digital pin 4
  #define botButton 39   // bottom decrease button connected to digital pin 8
#else
  #define topButton 2   // top mode button connected to digital pin 2
  #define midButton 4   // middle increase button connected to digital pin 4
  #define botButton 8   // bottom decrease button connected to digital pin 8
#endif

//const unsigned long uploadInterval = (300L * 1000L);  // Every 5 minutes upload sensor data
const unsigned long uploadInterval = (60L * 1000L); // for testing purposes
const unsigned long measureInterval = (30L * 1000L);  // Every 30 seconds measure the sensors
const unsigned long updateLCDInterval = (2L * 1000L); // Every 2 seconds update the LCD unless there is a change
const unsigned long updateDaytimeInterval = (30L * 1000L); // Every 30 seconds update time of day (and auto update configs)
const unsigned long updatePIDvalsInterval = (300L * 1000L); // Every 30 minutes (1800s) update values in EEPROM

unsigned char runMode = 0;  // 0 - run, 1 - edit setT, 2 - edit setH, -- more to come later...
unsigned char maxRunMode = 3;
unsigned char oldRunMode = 0;
// also need to be able to change the "fruitFlag" so light can go on and off during daytime

// Network Globals
byte mac[] = { 0x00, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE };  // registered on my router
// IPAddress ip(192, 168, 1, 223); // just in case DHCP doesn't work
// IPAddress dnServer(192, 168, 1, 254);
// IPAddress gateway(192, 168, 1, 254);

IPAddress ip(10, 80, 80, 180); // just in case DHCP doesn't work
IPAddress dnServer(10, 80, 80, 1);
IPAddress gateway(10, 80, 80, 1);

IPAddress subnet(255, 255, 255, 0);

IPAddress influxserver(35,197,35,96);
unsigned int udpport = 8089;

// Sensor Globals
#define DHTTYPE DHT22   // DHT 22  (AM2302)

// globals to save space as 10x real values... used for PID
unsigned int curT1=0, curT2=0, curT=0;
unsigned char curH1=0, curH2=0, curH=0;

// Beginning of active controls, these are the set points for the temp and humidity

float Erf;

#if defined(FIRSTTIME) &&  FIRSTTIME
// only set these defaults if first time setting up device.
  unsigned int setT = 2500; // two decimal places means multiply by 100 for accuracy 2560 (78F) for incubation, 2230 (72F)for fruiting
  unsigned char setH = 50; // initial set points for feedback
  boolean fruitFlag = true;  // used to determine whether the LED light is turned on and off in the daytime
  unsigned char hchID = 9; // influx seems to accomodate only a two digit ID here...
  float intErf=0;
  unsigned char heaterPower = 0; // make sure the heater starts in the off position
  boolean heatcoolFlag = false;
#else
  unsigned int setT;
  unsigned char setH;
  boolean fruitFlag;
  unsigned char hchID;
  float intErf;
  unsigned char heaterPower;
  boolean heatcoolFlag;
#endif

boolean lightFlag = false;  // should we turn on the LED
boolean mistFlag = false;   // should we mist?
boolean netFlag = false;    // used to notify if data upload has succeeded

boolean isDay = false;  // within the right hour range to be day time

// Connect via i2c, default address 0x27 (A0-A2 not jumpered)
LiquidCrystal_I2C lcd(0x27, 20, 4);  // Set the LCD I2C address

// initiate the buttons
Button topB(topButton, false, false, 50);    //Declare the button with no pullups, high on press, and 50ms debounce
Button midB(midButton, false, false, 50);
Button botB(botButton, false, false, 50);

EthernetClient eclient; // Instantiate the Ethernet Client Library
EthernetUDP udp;  // try sending influx data using UDP packets

int Hours=0;
int Minutes=0;

SimpleTimer timer;  // Instantiate the SimpleTimer object

DHT dht1(sensPin1, DHTTYPE); DHT dht2(sensPin2, DHTTYPE); DHT dht3(sensPin3, DHTTYPE);  //// Instantiate DHT sensors
void readEEPROM() {
  int eeAddress = 0;

  EEPROM.get(eeAddress, setT);
  eeAddress += sizeof(setT);
  EEPROM.get(eeAddress, setH);
  eeAddress += sizeof(setH);
  EEPROM.get(eeAddress, fruitFlag);
  eeAddress += sizeof(fruitFlag);
  //hchID added in 2021
  EEPROM.get(eeAddress, hchID);
  eeAddress += sizeof(hchID);
  // added to make the feedback less glitchy on reboot
  //float Erf, intErf;
  //EEPROM.get(eeAddress, intErf);
  //eeAddress += sizeof(intErf);
}

void updateEEPROM() {
  int eeAddress = 0;

  EEPROM.put(eeAddress, setT);
  eeAddress += sizeof(setT);
  EEPROM.put(eeAddress, setH);
  eeAddress += sizeof(setH);
  EEPROM.put(eeAddress, fruitFlag);
  eeAddress += sizeof(fruitFlag);
  //hchID added in 2021
  EEPROM.put(eeAddress, hchID);
  eeAddress += sizeof(hchID);
  // added to make the feedback less glitchy on reboot
  //float Erf, intErf;
  //EEPROM.put(eeAddress, intErf);
  //eeAddress += sizeof(intErf);
}

void readPIDvals() {
  int eeAddress = 0;
  eeAddress += sizeof(setT);
  eeAddress += sizeof(setH);
  eeAddress += sizeof(fruitFlag);
  eeAddress += sizeof(hchID);
  // intErf, heaterPower, heatcoolFlag, lightFlag
  EEPROM.get(eeAddress, intErf);
  eeAddress += sizeof(intErf);
  EEPROM.get(eeAddress, heaterPower);
  eeAddress += sizeof(heaterPower);
  EEPROM.get(eeAddress, heatcoolFlag);
  eeAddress += sizeof(heatcoolFlag);
  EEPROM.get(eeAddress, lightFlag);
  eeAddress += sizeof(lightFlag);

}
void updatePIDvals() {
  int eeAddress = 0;
  eeAddress += sizeof(setT);
  eeAddress += sizeof(setH);
  eeAddress += sizeof(fruitFlag);
  eeAddress += sizeof(hchID);
  // intErf, heaterPower, heatcoolFlag, lightFlag
  EEPROM.put(eeAddress, intErf);
  eeAddress += sizeof(intErf);
  EEPROM.put(eeAddress, heaterPower);
  eeAddress += sizeof(heaterPower);
  EEPROM.put(eeAddress, heatcoolFlag);
  eeAddress += sizeof(heatcoolFlag);
  EEPROM.put(eeAddress, lightFlag);
  eeAddress += sizeof(lightFlag);


}

void(* resetFunc) (void) = 0; //declare reset function @ address 0

void watchdogSetup(void) {
  cli(); // disable all interrupts
  wdt_reset(); // reset the WDT timer
  /*
  WDTCSR configuration:
  WDIE = 1: Interrupt Enable
  WDE = 1 :Reset Enable
  WDP3 = 1 :For 8000ms Time-out
  WDP2 = 0 :For 8000ms Time-out
  WDP1 = 0 :For 8000ms Time-out
  WDP0 = 1 :For 8000ms Time-out
  */
  // Enter Watchdog Configuration mode:
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  // Set Watchdog settings:
  WDTCSR = (1<<WDIE) | (1<<WDE) | (1<<WDP3) | (0<<WDP2) | (0<<WDP1) | (1<<WDP0);
  sei();
}


ISR(WDT_vect) { // Watchdog timer interrupt.
// Include your code here - be careful not to use functions that may cause the interrupt to hang and
// prevent a reset.
  resetFunc();  //call reset
}

void lcdSetup(void) {

  lcd.createChar(net1_char, N1_c);
  lcd.createChar(net2_char, N2_c);
  lcd.createChar(degrees_C_char, degC_c);

  lcd.backlight();
  delay(250);
  lcd.noBacklight();
  delay(250);
  lcd.backlight();
  delay(250);
  lcd.setCursor(0, 0);lcd.print(F("HCH  "));
  lcd.setCursor(0, 1);lcd.print(F("Mode:Setup    "));

}

void updateLCD(){

  //updateEEPROM(); // this is first time only, so that EEPROM has OK values
  readEEPROM();

  unsigned char b1; // two decimals now...
  unsigned char b2;

  if (runMode!=oldRunMode) {lcd.clear();}

  lcd.setCursor(0,0);
  if (Hours<10) {lcd.print("0");lcd.setCursor(1,0);}
  lcd.print(Hours);
  lcd.setCursor(2,0);lcd.print(F(":"));
  lcd.setCursor(3,0);
  if (Minutes<10) {lcd.print("0");lcd.setCursor(4,0);}
  lcd.print(Minutes);
  //lcd.setCursor(0, 0);lcd.print(F("HCH  "));
  lcd.setCursor(0, 1);lcd.print(F("Mode:"));
  if (heatcoolFlag) {lcd.setCursor(6, 0);lcd.print(F("HEAT"));
  } else {lcd.setCursor(6, 0);lcd.print(F("COOL"));}
  lcd.setCursor(10, 0);lcd.print(F("   "));

  lcd.setCursor(10, 0);
  if (heaterPower<100) {
    lcd.print(F("0"));lcd.setCursor(11, 0);
  }
  if (heaterPower<10) {
    lcd.print(F("0"));lcd.setCursor(12, 0);
  }
  lcd.print(heaterPower);

  if (mistFlag) {lcd.setCursor(14, 1);lcd.print(F("  MIST"));
  } else {lcd.setCursor(14, 1);lcd.print(F("noMIST"));}

  if (netFlag) {
    lcd.setCursor(18, 0);lcd.print(char(net1_char));  // print the cool Ethernet symbol on the screen
    lcd.setCursor(19, 0);lcd.print(char(net2_char));  // This means comms is good
  } else {
    lcd.setCursor(18, 0);lcd.print(F("--"));  // print the Eth fail on LCD
  }

  if (isDay) {
    lcd.setCursor(14, 0);lcd.print(F("DAY"));
  } else {
    lcd.setCursor(14, 0);lcd.print(F("EVE"));
  }

  if ((runMode>0) && (runMode<=maxRunMode)) {
    lcd.setCursor(5, 1);lcd.print(F("Config  "));
    //lcd.setCursor(5, 1);lcd.print(runMode,DEC);
    b1 = (unsigned char) (setT / 100); // two decimals now...
    b2 = (unsigned char) (setT - (b1 * 100));
    lcd.setCursor(0,2);lcd.print(F("setT="));lcd.setCursor(5, 2);lcd.print(b1);
    lcd.setCursor(7,2);lcd.print(F("."));lcd.setCursor(8, 2);lcd.print(b2);
    lcd.setCursor(10,2);lcd.print(char(degrees_C_char));  //moved to allow for 2 decimals
    lcd.setCursor(0,3);lcd.print(F("setH="));lcd.setCursor(5, 3);lcd.print(setH);
    lcd.setCursor(7,3);lcd.print(F("%"));
    lcd.setCursor(13,3);
    if (fruitFlag) {lcd.print("  Fruit");} else {lcd.print("noFruit");}
  }

  switch (runMode) {
    case 1:
      lcd.setCursor(12, 2);lcd.print(F("<-"));
      lcd.setCursor(10, 3);lcd.print(F("  "));
      break;
    case 2:
      lcd.setCursor(12, 2);lcd.print(F("  "));
      lcd.setCursor(10, 3);lcd.print(F("<-"));
      break;
    case 3:
      lcd.setCursor(12, 2);lcd.print(F("  "));
      lcd.setCursor(10, 3);lcd.print(F("->"));
      break;
    case 0:
      //lcd.clear();
      lcd.setCursor(5,1);lcd.print(F("Run     "));
      b1 = (unsigned char) (curT1 / 100) ; // two decimals now...
      b2 = (unsigned char) (curT1 - (b1 * 100)) ;
      lcd.setCursor(0, 2);lcd.print(F("T1="));lcd.setCursor(3, 2);lcd.print(b1);
      lcd.setCursor(5, 2);lcd.print(F("."));lcd.setCursor(6, 2);lcd.print(b2);
      lcd.setCursor(8, 2);lcd.print(char(degrees_C_char));  //moved to allow for 2 decimals

      lcd.setCursor(0, 3);lcd.print(F("H1="));lcd.setCursor(3, 3);lcd.print(curH1);
      lcd.setCursor(5, 3);lcd.print(F("%"));

      b1 = (unsigned char) (curT2 / 100);
      b2 = (unsigned char) (curT2 - (b1 * 100)) ;

      lcd.setCursor(11, 2);lcd.print(F("T2="));lcd.setCursor(14, 2);lcd.print(b1);
      lcd.setCursor(16, 2);lcd.print(F("."));lcd.setCursor(17, 2);lcd.print(b2);
      lcd.setCursor(19, 2);lcd.print(char(degrees_C_char)); //moved to allow for 2 decimals

      lcd.setCursor(11, 3);lcd.print(F("H2="));lcd.setCursor(14, 3);lcd.print(curH2);
      lcd.setCursor(16, 3);lcd.print(F("%"));
      break;
  }
  oldRunMode=runMode;
}

void ioSetup(void) {

  pinMode(sensPin1, INPUT_PULLUP); // DHT22 #1
  pinMode(sensPin2, INPUT_PULLUP); // DHT22 #2
  pinMode(sensPin3, INPUT_PULLUP); // DHT22 #3

  pinMode(topButton, INPUT);      // sets the topmost mode button digital pin as input
  pinMode(midButton, INPUT);      // sets the middle increase button digital pin as input
  pinMode(botButton, INPUT);      // sets the bottom decrease button digital pin as input
  // control relays
  pinMode(lightPin, OUTPUT);     // setup LED pin for relay control
  pinMode(mistPin, OUTPUT);     // setup mist pin for relay control
  pinMode(waterPumpPin, OUTPUT);     // setup Water pump pin for output to small h-bridge
  pinMode(peltierPwmPin, OUTPUT);   //
  pinMode(heatCoolPin, OUTPUT);   //
  // initial settings for the relays
  digitalWrite(lightPin, Relay_OFF);   // default setting on boot should be off
  digitalWrite(mistPin, Relay_OFF);   // default setting on boot should be off
  digitalWrite(heatCoolPin, Relay_OFF); // set to cool at bootup
  analogWrite(peltierPwmPin, 0);  // make sure peltier is off on bootup
}
/*
 * © Francesco Potortì 2013 - GPLv3
 *
 * Send an HTTP packet and wait for the response, return the Unix time
 * Adapted by Roland Clarke to merely return a boolean daytime indicator
 */

void isDaytime () {
  //eclient.flush();eclient.stop();
  // Decided to choose my own webserver for simplicity
  if (eclient.connect("rmclarke.ca", 80))  {
    // Make an HTTP 1.1 request which is missing a Host: header
    // compliant servers are required to answer with an error that includes
    // a Date: header.
    eclient.println(F("GET / HTTP/1.1"));
    eclient.println(F("Host: rmclarke.ca"));
    eclient.println(F("Connection: close"));
    eclient.println();

    char buf[5];      // temporary buffer for characters
    //eclient.setTimeout(500);
    if (eclient.find((char *)"\r\nDate: ") && eclient.readBytes(buf, 5) == 5) {
      //Serial.println(F("Date Found"));
      unsigned int day = eclient.parseInt();    // day
      eclient.readBytes(buf, 1);    // discard
      eclient.readBytes(buf, 3);    // month
      int year = eclient.parseInt();    // year
      Hours = eclient.parseInt();   // hour
      Minutes = eclient.parseInt(); // minute
      byte second = eclient.parseInt(); // second
    } //else {Serial.println(F("No Date"));}
    netFlag = true;
  } else {
    netFlag = false;
    Hours = 0;
    Minutes = 0;
  }
  delay(100);
  eclient.flush();
  eclient.stop();
  if (Hours>=7) {Hours = Hours-7;} else {Hours = Hours+17;} // adjust for PST time zone
  // adjust these numbers to change lighting time. 6am to 7pm initially.
  if ((Hours>=6) && (Hours<19))  {isDay = true;} else {isDay=false;}
  if (isDay && fruitFlag) {lightFlag = true;} else {lightFlag = false;}
}
void setHeater() {

  // Heater now controlled using pwm value of pin 3 - still experimental

  // be prepared to monkey with this to set the polarity properly visa vis the pololu bridge
  if (heatcoolFlag) {
    digitalWrite(heatCoolPin, LOW);
  } else {
    digitalWrite(heatCoolPin, HIGH); // If you want cooling, reverse polarity of H-bridge
  }
  // set peltier to proper pwm value for H-bridge...
  unsigned int pwmValue = (unsigned int) (lround(min(heaterPower * 255 / 100,255))); // max PWM value is 255
  analogWrite(peltierPwmPin, pwmValue);
  //digitalWrite(heatCoolPin, HIGH);
  //analogWrite(peltierPwmPin, 250);

}

void setMist() {
  if (mistFlag) {
    digitalWrite(mistPin, Relay_ON);
    delay(250);
  } else {
    digitalWrite(mistPin, Relay_OFF);
    delay(250);
  }
}

void setLED() {
  if (lightFlag) {
    digitalWrite(lightPin, Relay_ON);
    #if defined(DEBUG) &&  DEBUG
      Serial.println("LED Relay ON");
    #endif
    delay(250);
  } else {
    digitalWrite(lightPin, Relay_OFF);
    #if defined(DEBUG) &&  DEBUG
      Serial.println("LED Relay OFF");
    #endif
    delay(250);
  }

}

/*          GET SENSOR DATA         */
void readSensorData() {

  // should put in some logic here in case a bad reading from one of the sensors...

  float lastErf = Erf;
  float lastintErf = intErf;

  float t1= dht1.readTemperature();
  float h1 = dht1.readHumidity();

  float t2= dht2.readTemperature();
  float h2 = dht2.readHumidity();
  //Print temp and humidity values to serial monitor
  //DEBUG_PRINT("Hum1: " + String(hum, DEC) + " %, Tem1: " + String(temp, DEC) + " degC");

  // save sensor data to globals
  curT1 = (unsigned int) lround(t1 * 100);
  curH1 = (unsigned int) lround(h1);

  curT2 = (unsigned int) lround(t2 * 100);
  curH2 = (unsigned int) lround(h2);

  curT = (unsigned int) (lround(((float)curT1 + (float)curT2)/2 ));
  curH = (unsigned int) (lround(((float)curH1 + (float)curH2)/2 ));

  Erf = (float) ((float) curT - (float) setT);

  intErf += Erf; // multiplyer may not be necessary here, added it to avoid infinite accumulation of errors
  float difErf = (Erf - lastErf);

  // Hard code the gain to start... will abstract this later...
  //float Signal = 0.5 * (Erf + difErf*6 + intErf/4);
  float Signal = 0.5 * (Erf + difErf*12 + intErf/8);

  #if defined(DEBUG) &&  DEBUG
  Serial.println("Erf: "+String(Erf));
  Serial.println("difErf: "+String(difErf));
  Serial.println("intErf: "+String(intErf));
  Serial.println("Signal: "+String(Signal));
  #endif

  if (Signal>0) {
    heatcoolFlag = false;  // if T>Tset, then cool
  } else {
    heatcoolFlag = true;  // if T<Tset, then heat
  }
  heaterPower = (unsigned int) (lround(min(abs(Signal),100)));

  unsigned int curH = lround(((float) curH1 + (float)curH2)/2);

  if (curH < (setH-5)) {
    mistFlag = true;
  } else if (curH>setH) {
    mistFlag = false;
  }

  #if defined(DEBUG) &&  DEBUG
  if ((t1 >= 5) && (t1 <= 40)) {
    //DEBUG_PRINTF("Sensor 1: Good");
    //DEBUG_PRINT("Hum1: " + String(curH1, DEC) + " %, Tmp1: " + String(curT1, DEC) + " degC");
  } else {
    //DEBUG_PRINTF("Sensor 1: Fail");
  }

  if ((t2 >= 5) && (t2 <= 40)) {
    //DEBUG_PRINTF("Sensor 2: Good");
    //DEBUG_PRINT("Hum2: " + String(curH2, DEC) + " %, Tmp2: " + String(curT2, DEC) + " degC");
  } else {
    //DEBUG_PRINTF("Sensor 2: Fail");
  }
  #endif

  setHeater();
  setMist();
  setLED();
}
/*                                      */
/*          rmclarke.ca influxdb comm function         */
void sendData() {
  // initialize comms variables
  char outBuf[24];
  char databuffer[6];
  char data[500]; // need to end reliance on arduino 'Strings'

  unsigned char Tb1 = (unsigned char) (curT1 / 100);
  unsigned char Tb2 = (unsigned char) ((unsigned int)curT1 - ((unsigned int)Tb1 * 100));

  strcpy(data, "temp1,owner=rolo,group=home,device=hc");
  snprintf(databuffer, 3, "%02d", hchID);strcat(data, databuffer);
  strcat(data, " value=");

  snprintf(databuffer, 3, "%02d", Tb1);strcat(data, databuffer);strcat(data, ".");
  snprintf(databuffer, 3, "%02d", Tb2);strcat(data, databuffer);strcat(data, "\n");

  Tb1 = (unsigned char) (curT2 / 100);
  Tb2 = (unsigned char) ((unsigned int)curT2 - ((unsigned int)Tb1 * 100)) ;

  strcat(data, "temp2,owner=rolo,group=home,device=hc");
  snprintf(databuffer, 3, "%02d", hchID);strcat(data, databuffer);
  strcat(data, " value=");

  snprintf(databuffer, 3, "%02d", Tb1);strcat(data, databuffer);strcat(data, ".");
  snprintf(databuffer, 3, "%02d", Tb2);strcat(data, databuffer);strcat(data, "\n");

  strcat(data, "hum1,owner=rolo,group=home,device=hc");
  snprintf(databuffer, 3, "%02d", hchID);strcat(data, databuffer);
  strcat(data, " value=");

  snprintf(databuffer, 3, "%02d", curH1);strcat(data, databuffer);strcat(data, "\n");

  strcat(data, "hum2,owner=rolo,group=home,device=hc");
  snprintf(databuffer, 3, "%02d", hchID);strcat(data, databuffer);
  strcat(data, " value=");

  snprintf(databuffer, 3, "%02d", curH2);strcat(data, databuffer);strcat(data, "\n");

  strcat(data, "heatcool,owner=rolo,group=home,device=hc");
  snprintf(databuffer, 3, "%02d", hchID);strcat(data, databuffer);
  strcat(data, " value=");

  snprintf(databuffer, 3, "%d", heatcoolFlag);strcat(data, databuffer);strcat(data, "\n");

  strcat(data, "mist,owner=rolo,group=home,device=hc");
  snprintf(databuffer, 3, "%02d", hchID);strcat(data, databuffer);
  strcat(data, " value=");

  snprintf(databuffer, 3, "%d", mistFlag);strcat(data, databuffer);strcat(data, "\n");

  strcat(data, "hpower,owner=rolo,group=home,device=hc");
  snprintf(databuffer, 3, "%02d", hchID);strcat(data, databuffer);
  strcat(data, " value=");

  snprintf(databuffer, 5, "%03d", heaterPower);strcat(data, databuffer);strcat(data, "\n");

  strcat(data, "light,owner=rolo,group=home,device=hc");
  snprintf(databuffer, 3, "%02d", hchID);strcat(data, databuffer);
  strcat(data, " value=");

  snprintf(databuffer, 3, "%d", lightFlag);strcat(data, databuffer);strcat(data, "\n");

  Tb1 = (unsigned char) (setT / 100);
  Tb2 = (unsigned char) ((unsigned int)setT - ((unsigned int)Tb1 * 100)) ;

  strcat(data, "setT,owner=rolo,group=home,device=hc");
  snprintf(databuffer, 3, "%02d", hchID);strcat(data, databuffer);
  strcat(data, " value=");

  snprintf(databuffer, 3, "%02d", Tb1);strcat(data, databuffer);strcat(data, ".");
  snprintf(databuffer, 3, "%02d", Tb2);strcat(data, databuffer);strcat(data, "\n");

  strcat(data, "setH,owner=rolo,group=home,device=hc");
  snprintf(databuffer, 3, "%02d", hchID);strcat(data, databuffer);
  strcat(data, " value=");

  snprintf(databuffer, 3, "%02d", setH);strcat(data, databuffer);strcat(data, "\n");

  // Turn this off in production mode
  #ifdef DEBUG
  Serial.print(data);
  #endif

  udp.beginPacket(influxserver, udpport);
  udp.print(data);
  udp.endPacket();

}

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

Major upgrades for using the Mega 2560 Pro Mini board which has more memory
Adding a compiler compare so the code may work on the old boards...


*/

#include <avr/wdt.h>
#include <SPI.h>
#include <Ethernet.h>
#include <SimpleTimer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include "Wire.h"
#include <LiquidCrystal_I2C.h>
#include <Button.h>
#include <EEPROM.h>

#define Mega2560 1
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
/*          MAIN LOOP         */
void loop() {
  topB.read();  //Read the buttons
  midB.read();
  botB.read();

  if (topB.wasPressed()) {  //If the button was pressed, rotate runMode
    runMode++;
    if (runMode>maxRunMode) {runMode=0;}
    lcd.clear();
    updateLCD();
  }
  if (runMode==1) { // Edit the setT of the feedback routine
    if (midB.wasPressed()) {
      setT = setT + 10;
      updateEEPROM();
      updateLCD();
    }
    if (botB.wasPressed()) {
      setT = setT - 10;
      updateEEPROM();
      updateLCD();
    }
  }  else if (runMode==2) { // Edit the setH of the feedback routine
    if (midB.wasPressed()) {
      setH = setH + 1;
      updateEEPROM();
      updateLCD();
    }
    if (botB.wasPressed()) {
      setH = setH - 1;
      updateEEPROM();
      updateLCD();
    }
  } else if (runMode==3) {
    if (midB.wasPressed() || botB.wasPressed()) {
      if (fruitFlag) {fruitFlag=false;} else {fruitFlag=true;}
      updateEEPROM();
      updateLCD();
    }
  }

  if (runMode>maxRunMode) {runMode=0;} // very odd behavior where runMode = 180???
  timer.run();  // timer initiation...
  wdt_reset();
} // end main loop
