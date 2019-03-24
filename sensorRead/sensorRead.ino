void setup()
{
  Serial.begin(115200);
}
//Global variable
const int receivePacketInfo = 2; //3 parameters: bytes received; source address; message content (start from 0)
String receiveMessage[receivePacketInfo] = {};
int node = 0; //The number of node in Thread network (start from 0)
//Functions
String receivePacket();
void sendPacket();
void formatPrint();
char getData();

void loop()
{
  while (!Serial)
  {
    //wait for serial port to connect.
  }
  //String mess = receivePacket();
  Serial.print("Day la message: ");
  //Serial.println(receiveMessage[3]);
  delay(1000);
}

void readTemp()
{
}

void receivePacket(char *packet)
{
  /*How-to-use
  receivePacket(packet);
  Examble:
  sendPacket("2 bytes from fdde:ad00:beef:0:cf3c:df09:f013:55a1 14152 Hello,World!");
              ! -> pointer = 0
                           ! -> pointer = 3
                                                                      ! -> pointer = 5
  */
  String stringPacket = String(*packet);
  int pointer = 0;
  for (int i = 0; i <= stringPacket.length(); i++)
  {
    String buffer = "";
    if (stringPacket[i] != " ")
    {
      buffer += stringPacket[i]; //when not reach " " symbol add char to buffer
    }
    else if (pointer == 0)
    {
      receiveMessage[0] = buffer; //assign buffer to byte number parameter
    }
    else if (pointer == 3)
    {
      receiveMessage[1] = buffer; //assign buffer to address parameter
    }
    else if (pointer == 5)
    {
      receiveMessage[2] = buffer; //assign buffer to message parameter
    }
    pointer++; //increase pointer to the beginning of a word
  }
}

void sendPacket(char *destIpv6Addr, char *message)
{
  /*How-to-use
  sendPacket("address in IPv6","message");
  Examble:
  sendPacket("fdde:ad00:beef::....","Hello,World!");
  */
  Serial.print("udp send ");
  Serial.print(destIpv6Addr);
  Serial.print(" 1212 "); //print the UDP port, this case uses port 1212
  Serial.print(message);
  Serial.write(0x03); //0x03 a.k.a "End of text" in UTF-8
  Serial.println();
}
