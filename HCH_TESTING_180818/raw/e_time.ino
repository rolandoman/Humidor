/*
 * © Francesco Potortì 2013 - GPLv3
 *
 * Send an HTTP packet and wait for the response, return the Unix time
 * Adapted by Roland Clarke to also return Hours and Minutes and Ethernet
 * eventually to handle all the subtleties of IoT command from server
 */

void isDaytime () {
  //eclient.flush();eclient.stop();
  // Decided to choose my own webserver for simplicity

  char getLine[100];char outBuf[114];
  char databuffer[14];
  unsigned long syncnum = unixtime / 100 ;
  char prehash[24];strcpy(prehash, "hch2poop");

  sprintf(databuffer, "%lu", syncnum);
  strcat(prehash, databuffer);
  //size_t numBytes = sizeof(prehash) - 1;
  size_t numBytes=16;

  uint32_t otp = CRC32::calculate(prehash, numBytes);
  char dev_otp[9]="";

  if (eclient.connect("rmclarke.ca", 80))  {
    // Make an HTTP 1.1 request compliant servers are required to answer with an error that includes a Date: header.

    strcpy(getLine, "/?dev=hch2&setT=");
    snprintf(databuffer, 6, "%u", setT);
    strcat(getLine, databuffer);
    strcat(getLine, "&setH=");
    snprintf(databuffer, 4, "%u", setH);
    strcat(getLine, databuffer);
    strcat(getLine, "&fruit=");
    snprintf(databuffer, 3, "%u", fruitFlag);
    strcat(getLine, databuffer);
    strcat(getLine, "&lastupdate=");
    sprintf(databuffer, "%lu", lastupdate);
    strcat(getLine, databuffer);
    strcat(getLine, "&otp=");
    sprintf(dev_otp, "%lX", otp);
    strcat(getLine, dev_otp);
    sprintf(outBuf,"GET %s HTTP/1.1",getLine);

    //eclient.println("GET /?dev=hch2&setT=&setH=&fruit=&lastupdate=&otp= HTTP/1.1");
    eclient.println(outBuf);

    eclient.println(F("Host: rmclarke.ca"));
    eclient.println(F("Connection: close"));
    eclient.println();

    char buf[5]="";      // temporary buffer for characters
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

      int daysInPrevMonths;
	     switch (buf[0])
	     {
	         case 'F': daysInPrevMonths =  31; break; // Feb
	         case 'S': daysInPrevMonths = 243; break; // Sep
	         case 'O': daysInPrevMonths = 273; break; // Oct
	         case 'N': daysInPrevMonths = 304; break; // Nov
	         case 'D': daysInPrevMonths = 334; break; // Dec
	         default:
	          if (buf[0] == 'J' && buf[1] == 'a')
		          daysInPrevMonths = 0;		// Jan
	          else if (buf[0] == 'A' && buf[1] == 'p')
		          daysInPrevMonths = 90;		// Apr
	          else switch (buf[2])
		          {
		          case 'r': daysInPrevMonths =  59; break; // Mar
		          case 'y': daysInPrevMonths = 120; break; // May
		          case 'n': daysInPrevMonths = 151; break; // Jun
		          case 'l': daysInPrevMonths = 181; break; // Jul
		          default: // add a default label here to avoid compiler warning
		          case 'g': daysInPrevMonths = 212; break; // Aug
		          }
	     }

	     // This code will not work after February 2100
	     // because it does not account for 2100 not being a leap year and because
	     // we use the day variable as accumulator, which would overflow in 2149
	     day += (year - 1970) * 365;	// days from 1970 to the whole past year
	     day += (year - 1969) >> 2;	// plus one day per leap year
	     day += daysInPrevMonths;	// plus days for previous months this year
	     if (daysInPrevMonths >= 59	// if we are past February
	        && ((year & 3) == 0))	// and this is a leap year
	     day += 1;			// add one day
	     // Remove today, add hours, minutes and seconds this month
	     unixtime = (((day-1ul) * 24 + Hours) * 60 + Minutes) * 60 + second;
    } //else {Serial.println(F("No Date"));}

    // still inside the request - here is where we parse the rest of the header
    char buf2[20]="";char temp[20]="";
    char s_dev[5]="";
    unsigned int s_setT;
    unsigned char s_setH;
    boolean s_fruit;
    unsigned long s_lastupdate;
    char s_otp[9]="";
    //lcd.setCursor(5,1);lcd.print(dev_otp);
    if (eclient.find((char *)"\r\niot: ")) {
      //lcd.setCursor(9,1);lcd.print(F("iot"));
      // OK, server is communicating, parse the message
      // this should work no matter what order the variables are received
      if (eclient.find((char *)"dev=")) {
        eclient.readBytes(buf2, 4);
        sprintf(s_dev,"%s",buf2);
      }
      //if (eclient.find((char *)"setT=")) {
      //  eclient.readBytes(buf2, 4);
      //  snprintf(temp,6,"%s",buf2);
      //  s_setT = (unsigned int) strtoul (temp, NULL, 0);
      //}
      //if (eclient.find((char *)"setH=")) {
      //  eclient.readBytes(buf2, 2);
      //  snprintf(temp,4,"%s",buf2);
      //  s_setH = (unsigned char) strtoul (temp, NULL, 0);
      //}
      //if (eclient.find((char *)"fruit=")) {
      //  eclient.readBytes(buf2, 1);
      //  snprintf(temp,3,"%s",buf2);
      //  s_fruit = (boolean) strtoul (temp, NULL, 0);
      //}
      //if (eclient.find((char *)"lastupdate=")) {
      //  eclient.readBytes(buf2, 10);
      //  snprintf(temp,12,"%s",buf2);
      //  s_lastupdate = strtoul (temp, NULL, 0);
      //}
      if (eclient.find((char *)"otp=")) {
        eclient.readBytes(s_otp, 8);
      }
      //sprintf(buf2, "%lu", lastupdate);
      //lcd.setCursor(0,0);lcd.print("            ");lcd.setCursor(0,0);lcd.print(buf2);
      //lcd.setCursor(0,1);lcd.print("          ");lcd.setCursor(0,1);lcd.print(s_otp);
      //lcd.setCursor(0,2);lcd.print("          ");lcd.setCursor(0,2);lcd.print(buf2);
      // confirm good comms, then compare who's lastupdate is bigger, then make a choice
      //if ((dev_otp == s_otp) && (device == s_dev) && (lastupdate < s_lastupdate)) {
        // server wins, update my shit and save it
      //  lastupdate = s_lastupdate;
      //  setT = s_setT;setH = s_setH;
      //  fruitFlag = s_fruit;
      //  updateEEPROM();

      //} else {
        // device wins, do nothing, hope that the server updates its shit
      //}
      //if ((strcmp(dev_otp,s_otp) == 0) && (strcmp(device,s_dev) == 0)) {
      if (strcmp(dev_otp,s_otp) == 0) {
        lcd.setCursor(9,1);lcd.print(F("iot"));
      }
    } // else { no iot found }

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
