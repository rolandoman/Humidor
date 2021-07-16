
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

const unsigned long uploadInterval = (300L * 1000L);  // Every 5 minutes upload sensor data
//const unsigned long uploadInterval = (60L * 1000L); // for testing purposes
const unsigned long measureInterval = (30L * 1000L);  // Every 30 seconds measure the sensors
const unsigned long updateLCDInterval = (2L * 1000L); // Every 2 seconds update the LCD unless there is a change
const unsigned long updateDaytimeInterval = (30L * 1000L); // Every 30 seconds update time of day (and auto update configs)
const unsigned long updateEEPROMInterval = (3600L * 1000L); // Every 60 minutes update values in EEPROM

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


#if defined(FIRSTTIME) &&  FIRSTTIME
// only set these defaults if first time setting up device.
unsigned int setT = 2550; // two decimal places means multiply by 100 for accuracy 2560 (78F) for incubation, 2230 (72F)for fruiting
unsigned char setH = 75; // initial set points for feedback
boolean fruitFlag = false;  // used to determine whether the LED light is turned on and off in the daytime
unsigned char hchID = 9; // influx seems to accomodate only a two digit ID here...
float Erf;
float intErf=0;
#else
unsigned int setT;
unsigned char setH;
boolean fruitFlag;
unsigned char hchID;
float Erf, intErf;
#endif

unsigned char heaterPower = 0; // make sure the heater starts in the off position
boolean heatcoolFlag = false;

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
