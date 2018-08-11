/*                                      */
/*          rmclarke.ca influxdb comm function         */
void sendData() {
  // initialize comms variables
  char outBuf[24];
  char databuffer[6];
  char data[500]; // need to end reliance on arduino 'Strings'

  // Make sure that the DHCP lease is current..
  Ethernet.maintain();
  // close any connection before send a new request.

  unsigned char Tb1 = (unsigned char) (curT1 / 100);
  unsigned char Tb2 = (unsigned char) ((unsigned int)curT1 - ((unsigned int)Tb1 * 100));

  //String data = String("temp1,owner=rolo,group=home,device=hch2 value=")+String(Tb1,DEC)+"."+String(Tb2,DEC)+"\n";
  strcpy(data, "temp1,owner=rolo,group=home,device=hch2 value=");
  snprintf(databuffer, 3, "%02d", Tb1);strcat(data, databuffer);strcat(data, ".");
  snprintf(databuffer, 3, "%02d", Tb2);strcat(data, databuffer);strcat(data, "\n");

  Tb1 = (unsigned char) (curT2 / 100);
  Tb2 = (unsigned char) ((unsigned int)curT2 - ((unsigned int)Tb1 * 100)) ;
  //data.concat(String("temp2,owner=rolo,group=home,device=hch2 value=")+String(Tb1,DEC)+"."+String(Tb2,DEC)+"\n");
  strcat(data, "temp2,owner=rolo,group=home,device=hch2 value=");
  snprintf(databuffer, 3, "%02d", Tb1);strcat(data, databuffer);strcat(data, ".");
  snprintf(databuffer, 3, "%02d", Tb2);strcat(data, databuffer);strcat(data, "\n");

  //data.concat(String("hum1,owner=rolo,group=home,device=hch2 value=")+String(curH1,DEC)+"\n");
  strcat(data, "hum1,owner=rolo,group=home,device=hch2 value=");
  snprintf(databuffer, 3, "%02d", curH1);strcat(data, databuffer);strcat(data, "\n");

  //data.concat(String("hum2,owner=rolo,group=home,device=hch2 value=")+String(curH2,DEC)+"\n");
  strcat(data, "hum2,owner=rolo,group=home,device=hch2 value=");
  snprintf(databuffer, 3, "%02d", curH2);strcat(data, databuffer);strcat(data, "\n");

  //data.concat(String("heatcool,owner=rolo,group=home,device=hch2 value=")+String(heatcoolFlag,DEC)+"\n");
  strcat(data, "heatcool,owner=rolo,group=home,device=hch2 value=");
  snprintf(databuffer, 3, "%d", heatcoolFlag);strcat(data, databuffer);strcat(data, "\n");

  //data.concat(String("mist,owner=rolo,group=home,device=hch2 value=")+String(mistFlag,DEC)+"\n");
  strcat(data, "mist,owner=rolo,group=home,device=hch2 value=");
  snprintf(databuffer, 3, "%d", mistFlag);strcat(data, databuffer);strcat(data, "\n");

  //data.concat(String("hpower,owner=rolo,group=home,device=hch2 value=")+String(heaterPower,DEC)+"\n");
  strcat(data, "hpower,owner=rolo,group=home,device=hch2 value=");
  snprintf(databuffer, 5, "%03d", heaterPower);strcat(data, databuffer);strcat(data, "\n");

  //data.concat(String("light,owner=rolo,group=home,device=hch2 value=")+String(lightFlag,DEC)+"\n");
  strcat(data, "light,owner=rolo,group=home,device=hch2 value=");
  snprintf(databuffer, 3, "%d", lightFlag);strcat(data, databuffer);strcat(data, "\n");

  Tb1 = (unsigned char) (setT / 100);
  Tb2 = (unsigned char) ((unsigned int)setT - ((unsigned int)Tb1 * 100)) ;
  //data.concat(String("setT,owner=rolo,group=home,device=hch2 value=")+String(Tb1,DEC)+"."+String(Tb2,DEC)+"\n");
  strcat(data, "setT,owner=rolo,group=home,device=hch2 value=");
  snprintf(databuffer, 3, "%02d", Tb1);strcat(data, databuffer);strcat(data, ".");
  snprintf(databuffer, 3, "%02d", Tb2);strcat(data, databuffer);strcat(data, "\n");

  //data.concat(String("setH,owner=rolo,group=home,device=hch2 value=")+String(setH,DEC)+"\n");
  strcat(data, "setH,owner=rolo,group=home,device=hch2 value=");
  snprintf(databuffer, 3, "%02d", setH);strcat(data, databuffer);strcat(data, "\n");

  udp.beginPacket(influxserver, udpport);
  udp.print(data);
  udp.endPacket();

}
