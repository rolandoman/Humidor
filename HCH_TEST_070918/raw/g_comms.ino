/*                                      */
/*          rmclarke.ca influxdb comm function         */
void sendData() {
  // initialize comms variables
  //char outBuf[24];
  char databuffer[6]="";
  char data[500]=""; // need to end reliance on arduino 'Strings'

  unsigned char Tb1 = (unsigned char) (curT1 / 100);
  unsigned char Tb2 = (unsigned char) ((unsigned int)curT1 - ((unsigned int)Tb1 * 100));

  strcpy(data, "temp1,owner=rolo,group=home,device=");strcat(data, device);strcat(data, " value=");
  snprintf(databuffer, 3, "%02d", Tb1);strcat(data, databuffer);strcat(data, ".");
  snprintf(databuffer, 3, "%02d", Tb2);strcat(data, databuffer);strcat(data, "\n");

  Tb1 = (unsigned char) (curT2 / 100);
  Tb2 = (unsigned char) ((unsigned int)curT2 - ((unsigned int)Tb1 * 100)) ;
  strcat(data, "temp2,owner=rolo,group=home,device=");strcat(data, device);strcat(data, " value=");
  snprintf(databuffer, 3, "%02d", Tb1);strcat(data, databuffer);strcat(data, ".");
  snprintf(databuffer, 3, "%02d", Tb2);strcat(data, databuffer);strcat(data, "\n");

  strcat(data, "hum1,owner=rolo,group=home,device=");strcat(data, device);strcat(data, " value=");
  snprintf(databuffer, 3, "%02d", curH1);strcat(data, databuffer);strcat(data, "\n");

  strcat(data, "hum2,owner=rolo,group=home,device=");strcat(data, device);strcat(data, " value=");
  snprintf(databuffer, 3, "%02d", curH2);strcat(data, databuffer);strcat(data, "\n");

  strcat(data, "heatcool,owner=rolo,group=home,device=");strcat(data, device);strcat(data, " value=");
  snprintf(databuffer, 3, "%d", heatcoolFlag);strcat(data, databuffer);strcat(data, "\n");

  strcat(data, "mist,owner=rolo,group=home,device=");strcat(data, device);strcat(data, " value=");
  snprintf(databuffer, 3, "%d", mistFlag);strcat(data, databuffer);strcat(data, "\n");

  strcat(data, "hpower,owner=rolo,group=home,device=");strcat(data, device);strcat(data, " value=");
  snprintf(databuffer, 5, "%03d", heaterPower);strcat(data, databuffer);strcat(data, "\n");

  strcat(data, "light,owner=rolo,group=home,device=");strcat(data, device);strcat(data, " value=");
  snprintf(databuffer, 3, "%d", lightFlag);strcat(data, databuffer);strcat(data, "\n");

  Tb1 = (unsigned char) (setT / 100);
  Tb2 = (unsigned char) ((unsigned int)setT - ((unsigned int)Tb1 * 100)) ;
  strcat(data, "setT,owner=rolo,group=home,device=");strcat(data, device);strcat(data, " value=");
  snprintf(databuffer, 3, "%02d", Tb1);strcat(data, databuffer);strcat(data, ".");
  snprintf(databuffer, 3, "%02d", Tb2);strcat(data, databuffer);strcat(data, "\n");

  strcat(data, "setH,owner=rolo,group=home,device=");strcat(data, device);strcat(data, " value=");
  snprintf(databuffer, 3, "%02d", setH);strcat(data, databuffer);strcat(data, "\n");

  udp.beginPacket(influxserver, udpport);
  udp.print(data);
  udp.endPacket();


}
