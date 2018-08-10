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

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // start the Ethernet connection:
  //if (Ethernet.begin(mac) == 0) {
  //  Serial.println("Failed to configure Ethernet using DHCP");
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns, gateway, subnet);
  //}
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("connecting...");

  char outBuf[64];
  String data = "temp1,owner=rolo,group=home,device=hch2 value=27.19\n";
  data = data + "temp2,owner=rolo,group=home,device=hch2 value=30.22";

  // if you get a connection, report back via serial:
  if (client.connect(server, 8086)) {
    Serial.println("connected");
    // Make a HTTP request:
    //client.println("GET / HTTP/1.1");
    client.println(F("POST /write?db=iot1&u=iot&p=iot HTTP/1.1"));
    client.println("Host: rmclarke.ca");
    client.println("Connection: close");
    client.println("Content-Type: multipart/form-data");
    //client.println("Content-Length: 52");
    sprintf(outBuf,"Content-Length: %u\r\n",data.length());
    client.println(outBuf);
    client.print(data);
    //client.println("temp1,owner=rolo,group=home,device=hch2 value=27.19");
    //client.println("temp2,owner=rolo,group=home,device=hch2 value=27.28");
    client.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}

void loop() {
  // if there are incoming bytes available
  // from the server, read them and print them:
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();

    // do nothing forevermore:
    while (true);
  }
}
