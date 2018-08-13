
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

  readEEPROMS();

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
