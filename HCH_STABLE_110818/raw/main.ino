/*          MAIN LOOP         */
void loop() {
  topB.read();  //Read the buttons
  midB.read();
  botB.read();

  if (topB.wasPressed()) {  //If the button was pressed, rotate runMode
    runMode++;
    if (runMode>2) {
      runMode=0;
    }
    lcd.clear();
    updateLCD();
  }
  //if (runMode==1) { // Edit the setT of the feedback routine
  //  if (midB.wasPressed()) {setT = setT + 10;updateLCD();}
  //  if (botB.wasPressed()) {setT = setT - 10;updateLCD();}
  //}  else if (runMode==2) { // Edit the setH of the feedback routine
  //  if (midB.wasPressed()) {setH = setH + 1;updateLCD();}
  //  if (botB.wasPressed()) {setH = setH - 1;updateLCD();}
  //}

  //eclient.flush();
  if (runMode>2) {runMode=0;} // very odd behavior where runMode = 180???
  timer.run();  // timer initiation...
  wdt_reset();
} // end main loop
