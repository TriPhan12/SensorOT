#include <string.h>
void setup()
{
  Serial.begin(115200);
}
//Global variable
String sourceAddress = "", receiveMessage = "";
int receiveByte = 0;
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
  Serial.println("Day la so byte: ");
  Serial.println(receivePacket("2 bytes from fdde:ad00:beef:0:cf3c:df09:f013:55a1 14152 Hello,World!", 1));
  Serial.println("Day la source address: ");
  Serial.println(receivePacket("2 bytes from fdde:ad00:beef:0:cf3c:df09:f013:55a1 14152 Hello,World!", 2));
  Serial.println("Day la message: ");
  Serial.println(receivePacket("2 bytes from fdde:ad00:beef:0:cf3c:df09:f013:55a1 14152 Hello,World!", 3));

  while (1)
  {
    delay(1000);
  }
}

String receivePacket(char *packet, int data)
{
  /*How-to-use
  receivePacket(packet);
  Examble:
  receivePacket("2 bytes from fdde:ad00:beef:0:cf3c:df09:f013:55a1 14152 Hello,World!", 1);
                 ! -> pointer = 0                                                       ! 1 for data at pointer 0
                              ! -> pointer = 3                                          ! 2 for data at pointer 3
                                                                         ! -> pointer = 5|3 for data at pointer 5
  */
  int pointer = 0;
  String tampont = "";
  int i = 0;
  switch (data)
  {
  case 1:
    pointer = 0;
    break;
  case 2:
    //Serial.println(data);
    pointer = 3;
    break;
  case 3:
    //Serial.println(data);
    pointer = 5;
    break;
  default:
    break;
  }
  while (pointer >= 0)
  {
    if ((pointer != 0)|| (packet[i] == ' '))
    {
      pointer = pointer - 1;
    }
    else if ((pointer == 0) || (packet[i] != ' '))
    {
      tampont = tampont + packet[i]; //when not reach " " symbol add char to buffer
      //Serial.println(tampont);      
    }
    else if ((pointer == 0) || (packet[i] == ' ')){
      pointer = pointer -1;
    }  
    i = i + 1;
  }
  return tampont;
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
