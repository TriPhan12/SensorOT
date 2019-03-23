
void setup()
{
  Serial.begin(115200);
}

void receivePacket();
void sendPacket();
static void formatPrint();

void loop()
{

  for (int i = 0; i < 5; i++)
  {
    sendPacket()
  }
}

void readTemp()
{
}

void receivePacket(char packetReceived[10])
{
}

void sendPacket(char *destIpv6Addr, char *message)
{
  //How-to-use
  //sendPacket("fdde:ad00:....","message");
  Serial.print("udp send")
  Serial.print(destIpv6Addr);
  Serial.print(" 1212 ");           //print the UDP port, this case uses port 1212
  Serial.println(message);
}
