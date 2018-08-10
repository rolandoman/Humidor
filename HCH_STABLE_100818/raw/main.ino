/*          MAIN LOOP         */
void loop() {
  topB.read();  //Read the buttons
  midB.read();
  botB.read();

  //if (isDaytime(eclient)) {lcd.setCursor(10, 3);lcd.print(F("DAY"));} else {lcd.setCursor(10, 3);lcd.print(F("EVE"));}
  //delay(2000);
  
  if (topB.wasPressed()) {runMode = runMode+1;if (runMode>2) {runMode=0;}lcd.clear();updateLCD();}  //If the button was pressed, say so...
  if (runMode==1) { // Edit the setT of the feedback routine
    if (midB.wasPressed()) {setT = setT + 10;updateLCD();}
    if (botB.wasPressed()) {setT = setT - 10;updateLCD();}
  }  
  if (runMode==2) { // Edit the setH of the feedback routine
    if (midB.wasPressed()) {setH = setH + 1;updateLCD();}
    if (botB.wasPressed()) {setH = setH - 1;updateLCD();}
  }  
  timer.run();  // timer initiation...
  wdt_reset(); 
} // end main loop


