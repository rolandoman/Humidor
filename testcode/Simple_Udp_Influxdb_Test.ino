/*
  Web client

 This sketch connects to a website (http://www.google.com)
 using an Arduino Wiznet Ethernet shield.

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13

 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe, based on work by Adrian McEwen

 */

#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0x00, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE };
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
char server[] = "rmclarke.ca";    // name address for Google (using DNS)

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 1, 223);
IPAddress myDns(192, 168, 1, 254);
IPAddress gateway(192, 168, 1, 254);
IPAddress subnet(255, 255, 255, 0);

IPAddress influxserver(35,197,35,96);
unsigned int udpport = 8089;

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient eclient;
EthernetUDP udp;

unsigned int curT1 = 2550;
unsigned int curT2 = 2640;


void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Ethernet.begin(mac, ip, myDns, gateway, subnet);
  udp.begin(udpport);
  delay(1000);
}

void loop() {
  sendData();
  delay(5000);
}


void sendData() {
  // initialize comms variables
  char outBuf[24];
  char databuffer[6];
  char data[500]; // need to end reliance on arduino 'Strings'

  unsigned char Tb1 = (unsigned char) (curT1 / 100);
  unsigned char Tb2 = (unsigned char) ((unsigned int)curT1 - ((unsigned int)Tb1 * 100));

  strcpy(data, "temp1,owner=rolo,group=home,device=hch2 value=");
  snprintf(databuffer, 3, "%02d", Tb1);strcat(data, databuffer);strcat(data, ".");
  snprintf(databuffer, 3, "%02d", Tb2);strcat(data, databuffer);
  Serial.println("connecting...");
  Serial.println(data);
  udp.beginPacket(influxserver, udpport);
  udp.print(data);
  udp.endPacket();
}


