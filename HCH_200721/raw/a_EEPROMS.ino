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
