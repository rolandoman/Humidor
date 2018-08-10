/*                                      */
/*          THINGSPEAK API CALL         */
void sendData() {
  // Make sure that the DHCP lease is current...
  Ethernet.maintain();
  // close any connection before send a new request.
  // This will free the socket on the Ethernet shield
  eclient.flush();eclient.stop();
  unsigned int Tb1 = (unsigned int) (curT1 / 100);
  unsigned int Tb2 = (unsigned int) ((unsigned int)curT1 - ((unsigned int)Tb1 * 100));
  // if there's a successful connection:
  if (eclient.connect("rmclarke.ca", 8086)) {

    char outBuf[4];
    String data = String(F("temp1,owner=rolo,group=home,device=hch2 value="))+String(Tb1,DEC)+"."+String(Tb2,DEC)+"\n";
    Tb1 = (unsigned int) (curT2 / 100);
    Tb2 = (unsigned int) ((unsigned int)curT2 - ((unsigned int)Tb1 * 100)) ;
    data.concat(String(F("temp2,owner=rolo,group=home,device=hch2 value="))+String(Tb1,DEC)+"."+String(Tb2,DEC)+"\n");
    data.concat(String(F("hum1,owner=rolo,group=home,device=hch2 value="))+String(curH1,DEC)+"\n");
    data.concat(String(F("hum2,owner=rolo,group=home,device=hch2 value="))+String(curH2,DEC)+"\n");
    data.concat(String(F("heatcool,owner=rolo,group=home,device=hch2 value="))+String(heatcoolFlag,DEC)+"\n");
    data.concat(String(F("mist,owner=rolo,group=home,device=hch2 value="))+String(mistFlag,DEC)+"\n");
    data.concat(String(F("hpower,owner=rolo,group=home,device=hch2 value="))+String(heaterPower,DEC)+"\n");
    data.concat(String(F("light,owner=rolo,group=home,device=hch2 value="))+String(lightFlag,DEC)+"\n");
    Tb1 = (unsigned int) (setT / 100);
    Tb2 = (unsigned int) ((unsigned int)setT - ((unsigned int)Tb1 * 100)) ;
    data.concat(String(F("setT,owner=rolo,group=home,device=hch2 value="))+String(Tb1,DEC)+"."+String(Tb2,DEC)+"\n");
    data.concat(String(F("setH,owner=rolo,group=home,device=hch2 value="))+String(setH,DEC)+"\n");
    eclient.println(F("POST /write?db=iot1&u=iot&p=iot HTTP/1.1"));
    eclient.println(F("Host: rmclarke.ca"));
    eclient.println(F("Connection: close"));
    eclient.println(F("Content-Type: multipart/form-data"));
    sprintf(outBuf,"Content-Length: %u\r\n",data.length());
    eclient.println(outBuf);
    eclient.print(data);
    eclient.println();
       
    netFlag = true;
    delay(10);
    eclient.flush();
    eclient.stop();

  } else {
    // if you couldn't make a connection:
    //DEBUG_PRINTF("data upload failed...");
    netFlag = false;
    // here is where we should put the reset code...
  }
}
