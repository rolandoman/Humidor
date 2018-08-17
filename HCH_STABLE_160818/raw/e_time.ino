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
    eclient.println(F("GET /?dev=hch2&setT=&setH=&fruit=&lastupdate=&otp= HTTP/1.1"));
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
	     time = (((day-1ul) * 24 + hour) * 60 + minute) * 60 + second;
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
