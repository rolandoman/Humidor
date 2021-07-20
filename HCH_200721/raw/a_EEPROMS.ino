void readEEPROM() {
  int eeAddress = 0;

  EEPROM.get(eeAddress, setT);
  eeAddress += sizeof(setT);
  EEPROM.get(eeAddress, setH);
  eeAddress += sizeof(setH);
  EEPROM.get(eeAddress, fruitFlag);
  eeAddress += sizeof(fruitFlag);
  //hchID added in 2021
  EEPROM.get(eeAddress, hchID);
  eeAddress += sizeof(hchID);
  // added to make the feedback less glitchy on reboot
  //float Erf, intErf;
  //EEPROM.get(eeAddress, intErf);
  //eeAddress += sizeof(intErf);
}

void updateEEPROM() {
  int eeAddress = 0;

  EEPROM.put(eeAddress, setT);
  eeAddress += sizeof(setT);
  EEPROM.put(eeAddress, setH);
  eeAddress += sizeof(setH);
  EEPROM.put(eeAddress, fruitFlag);
  eeAddress += sizeof(fruitFlag);
  //hchID added in 2021
  EEPROM.put(eeAddress, hchID);
  eeAddress += sizeof(hchID);
  // added to make the feedback less glitchy on reboot
  //float Erf, intErf;
  //EEPROM.put(eeAddress, intErf);
  //eeAddress += sizeof(intErf);
}

void readPIDvals() {
  int eeAddress = 0;
  eeAddress += sizeof(setT);
  eeAddress += sizeof(setH);
  eeAddress += sizeof(fruitFlag);
  eeAddress += sizeof(hchID);
  // intErf, heaterPower, heatcoolFlag, lightFlag
  EEPROM.get(eeAddress, intErf);
  eeAddress += sizeof(intErf);
  EEPROM.get(eeAddress, heaterPower);
  eeAddress += sizeof(heaterPower);
  EEPROM.get(eeAddress, heatcoolFlag);
  eeAddress += sizeof(heatcoolFlag);
  EEPROM.get(eeAddress, lightFlag);
  eeAddress += sizeof(lightFlag);

}
void updatePIDvals() {
  int eeAddress = 0;
  eeAddress += sizeof(setT);
  eeAddress += sizeof(setH);
  eeAddress += sizeof(fruitFlag);
  eeAddress += sizeof(hchID);
  // intErf, heaterPower, heatcoolFlag, lightFlag
  EEPROM.put(eeAddress, intErf);
  eeAddress += sizeof(intErf);
  EEPROM.put(eeAddress, heaterPower);
  eeAddress += sizeof(heaterPower);
  EEPROM.put(eeAddress, heatcoolFlag);
  eeAddress += sizeof(heatcoolFlag);
  EEPROM.put(eeAddress, lightFlag);
  eeAddress += sizeof(lightFlag);


}
