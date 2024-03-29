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
  float Signal = 0.5 * (Erf + difErf*8 + intErf/12);

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
