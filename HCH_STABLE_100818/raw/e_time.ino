/*
 * © Francesco Potortì 2013 - GPLv3
 *
 * Send an HTTP packet and wait for the response, return the Unix time
 * Adapted by Roland Clarke to merely return a boolean daytime indicator
 */

void isDaytime () {

  // Decided to choose my own webserver for simplicity
  if (eclient.connect("rmclarke.ca", 80))  {
    // Make an HTTP 1.1 request which is missing a Host: header
    // compliant servers are required to answer with an error that includes
    // a Date: header.
    eclient.println("GET / HTTP/1.1");
    eclient.println("Host: rmclarke.ca");
    eclient.println("Connection: close");
    eclient.println();

    char buf[5];      // temporary buffer for characters
    eclient.setTimeout(500);
    if (eclient.find((char *)"\r\nDate: ") && eclient.readBytes(buf, 5) == 5) {
      //Serial.println(F("Date Found"));
      unsigned day = eclient.parseInt();    // day
      eclient.readBytes(buf, 1);    // discard
      eclient.readBytes(buf, 3);    // month
      int year = eclient.parseInt();    // year
      Hours = eclient.parseInt();   // hour
      Minutes = eclient.parseInt(); // minute
      byte second = eclient.parseInt(); // second
    } //else {Serial.println(F("No Date"));}
    netFlag = true;
  } else {netFlag = false;}
  delay(10);
  eclient.flush();
  eclient.stop();
  if (Hours>=7) {Hours = Hours-7;} else {Hours = Hours+17;} // adjust for PST time zone
  // adjust these numbers to change lighting time. 6am to 7pm initially.
  if (((Hours>=6) && (Hours<19)) && fruitFlag)  {lightFlag = true;} else {lightFlag = false;}
}
