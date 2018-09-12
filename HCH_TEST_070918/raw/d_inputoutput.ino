
void ioSetup(void) {

  pinMode(sensPin1, INPUT_PULLUP);
  pinMode(sensPin2 INPUT_PULLUP);
  pinMode(sensPin3, INPUT_PULLUP);

  pinMode(topButton, INPUT);      // sets the topmost mode button digital pin as input
  pinMode(midButton, INPUT);      // sets the middle increase button digital pin as input
  pinMode(botButton, INPUT);      // sets the bottom decrease button digital pin as input
  // control relays
  pinMode(lightPin, OUTPUT);     // setup LED pin for relay control
  pinMode(mistPin, OUTPUT);     // setup mist pin for relay control
  pinMode(waterPumpPin, OUTPUT);     // setup Water pump pin for output to small h-bridge
  pinMode(peltierPwmPin, OUTPUT);   //
  pinMode(heatCoolPin, OUTPUT);   //
  // initial settings for the relays
  digitalWrite(lightPin, Relay_OFF);   // default setting on boot should be off
  digitalWrite(mistPin, Relay_OFF);   // default setting on boot should be off
  digitalWrite(heatCoolPin, Relay_OFF); // set to cool at bootup
  analogWrite(peltierPwmPin, 0);  // make sure peltier is off on bootup
  
}
