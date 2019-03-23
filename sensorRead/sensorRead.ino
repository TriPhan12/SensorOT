void setup()
{
  Serial.begin(115200);
}

void receivePacket();
void sendPacket();
void formatPrint();
//char getData(*char packet);

void loop()
{
  for (int i = 0; i <= 2; i++)
  {
    sendPacket("fdde:ad00:beef::","temp1");
    delay(100);
  }
  
}

void readTemp()
{
}

void receivePacket(char *packet)
{    
  //char receiveData = getData(*packet);

}

void sendPacket(char *destIpv6Addr, char *message)
{
  //How-to-use
  //sendPacket("fdde:ad00:....","message");
  Serial.print("udp send ");
  Serial.print(destIpv6Addr);
  Serial.print(" 1212 ");           //print the UDP port, this case uses port 1212
  Serial.print(message);  
  Serial.write(0x03);             //0x03 a.k.a "End of text" in UTF-8
  Serial.println();
}
