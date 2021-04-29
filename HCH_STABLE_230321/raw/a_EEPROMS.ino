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
}
