#include <SPI.h>
#include <Ethernet.h>
#include <SimpleTimer.h>
#include <DHT.h>;

#define DEBUG
#ifdef DEBUG
 #define DEBUG_PRINT(x)  Serial.println (x)
#else
 #define DEBUG_PRINT(x)
#endif

const unsigned long uploadInterval = (600L * 1000L);	// Every 10 minutes
const unsigned long measureInterval = (30L * 1000L);	// Every 30 seconds

// Network Globals
byte mac[] = { 0x00, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };	// registered on my router
IPAddress ip(192, 168, 1, 222);	// just in case DHCP doesn't work
EthernetClient eclient;	// Instantiate the Ethernet Client Library
char server[] = "api.thingspeak.com";

SimpleTimer timer;	// Instantiate the timer object

// Sensor Globals
#define sensor1 A0
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht1(sensor1, DHTTYPE); //// Instantiate DHT sensor
unsigned int curT1;	// globals to save space as 10x real values
unsigned int curH1;

/*										SETUP FUNCTION										*/
void setup() {
	// start serial port:
	Serial.begin(9600);
	while (!Serial) {; // wait for serial port to connect. Needed for native USB port only
	}	
	delay(1000);	// give the ethernet module time to boot up:
	
	if (Ethernet.begin(mac) == 0) {	// start the Ethernet connection:
		Serial.println("Failed to configure Ethernet using DHCP");
		// try to congifure using IP address instead of DHCP:
		Ethernet.begin(mac, ip);
	}

	// give the Ethernet shield a second to initialize:
	Serial.println("connecting...");	
	delay(1000);
  // print the Ethernet board/shield's IP address:
  DEBUG_PRINT("My IP address: " + Ethernet.localIP());
	
	dht1.begin();
	
	timer.setInterval(uploadInterval, apiRequest);	
	timer.setInterval(measureInterval, getSensorData);	
  
}	// end setup

/*										MAIN LOOP										*/
void loop() {
  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  if (eclient.available()) {
    char c = eclient.read();
    Serial.write(c);
  }

	timer.run();	// timer initiation...

}	// end main loop

/*										THINGSPEAK API CALL										*/
void apiRequest() {
	// close any connection before send a new request.
  // This will free the socket on the Ethernet shield
  eclient.stop();

	unsigned int bit1 = curT1 / 10 ;
	unsigned int bit2 = curT1 - (bit1 * 10) ;
  String getln = "GET /update?api_key=P7YQA0CK2XSQO9J5&field1=" + String(bit1, DEC) + "." + String(bit2, DEC);

  // if there's a successful connection:
  if (client.connect(server, 80)) {
    DEBUG_PRINT("connecting...");
    // send the HTTP GET request:
    DEBUG_PRINT(getln);
    eclient.println(getln);
    eclient.println("Host: api.thingspeak.com");
    eclient.println("User-Agent: arduino-ethernet");
    eclient.println("Connection: close");
    eclient.println();

  } else {
    // if you couldn't make a connection:
    DEBUG_PRINT("data upload failed...");
  }
}

/*										GET SENSOR DATA										*/
void getSensorData() {
	float temp= dht1.readTemperature();
	float hum = dht1.readHumidity();
  
	//Print temp and humidity values to serial monitor
  DEBUG_PRINT("Hum1: " + String(hum, DEC) + " %, Tem1: " + String(temp, DEC) + " degC");
	
	// save sensor data to globals
	curT1 = (unsigned int) lround(temp * 10);
	curH1 = (unsigned int) lround(hum * 10);
	
}




