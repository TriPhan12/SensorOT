#include <DHT.h>

//Setup for the temperature sensor DHT11
#define tempProbe 2
#define DHTTYPE DHT11
DHT dht(tempProbe,DHTTYPE);

void setup()
{
  Serial.begin(115200);  

  dht.begin();
}
//Global variable
String sourceAddress = "", receiveMessage = "";
int receiveByte = 0;


//Functions
void guiDoam();
String receivePacket();
void sendPacket();
void formatPrint();
char getData();
void tempget();
float humiget();

void sendPacket2(char *destIpv6Addr, char *message)
{
/*How-to-use
sendPacket("address in IPv6","message");
Examble:
sendPacket("fdde:ad00:beef::....","Hello,World!");
*/
  // Serial.print("udp send ");
  Serial.print(destIpv6Addr);
  // Serial.print(" 1212 "); //print the UDP port, this case uses port 1212
  Serial.println(message);
  //Serial.write(0x03); //0x03 a.k.a "End of text" in UTF-8
}

void loop()
{
  while (!Serial)
  {
    //wait for serial port to connect.
  }
  
  delay(1000);
//  guiDoam();
  delay(2000);
  sendPacket2("2 bytes from fdde:ad00:beef:0:cf3c:df09:f013:55a1 14152", " humi_12.56");
}

void guiDoam(){  
  float doam = humiget();
  //Serial.println(doamChuoi);
  String doamChuoi = "doam";
  doamChuoi = doamChuoi + doam;
  doamChuoi = "doam_36.23";
  char chuoiXuat;
  doamChuoi.toCharArray(chuoiXuat, 9);
  // Serial.println(chuoiXuat);
  doamChuoi = "";
  sendPacket("fdde:ad00:beef:0:cf3c:df09:f013:55a1", chuoiXuat );
}

String receivePacket(char *packet, int data)
{
  /*How-to-use
  receivePacket(packet);
  Examble:
  receivePacket("2 bytes from fdde:ad00:beef:0:cf3c:df09:f013:55a1 14152 Hello,World!", 1);
                 ! -> pointer = 0                                                       ! 1 for data at pointer 0 (bytes sent)
                              ! -> pointer = 3                                          ! 2 for data at pointer 3 (source address)
                                                                         ! -> pointer = 5|3 for data at pointer 5 (message)
  */
  int pointer = 5;
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
  default:
    //Serial.println(data);
    pointer = 5;
    break;
  }

  while (pointer > 0)
  {
    if (packet[i] == ' ') //if appeare a ' ' in sentence reduce pointer => enter a new word
    {
      pointer = pointer - 1; //decrease pointer by 1 to get to value 0
    }
    i = i + 1; //increase i to go to next charactor
  }
  while (pointer == 0)
  {
    if (packet[i] != ' ')
    {
      tampont = tampont + packet[i]; //when not reach " " symbol add char to buffer
      //Serial.println(tampont);
    }
    else
    {
      pointer = pointer - 1; // decrease pointer to value -1 to escape from the while loop
    }
    i = i + 1; //increase i to go to next charactor
  }
  if (data == 3) //only remove 3 charactors at the end of the message
  {
    tampont.remove(tampont.length() - 3, 3);
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
  Serial.println(message);
  //Serial.write(0x03); //0x03 a.k.a "End of text" in UTF-8
}

void tempget(){
  float temp = dht.readTemperature();
  /*
  if (isnan(temp)) {
          Serial.println("Failed to read temperature from DHT sensor!");
  }else{
          Serial.print("Nhiet do ");
          Serial.print(String(temp, 1).c_str());
          Serial.print("      Do am ");
          Serial.println(String(humi, 1).c_str());
  }  
  //return temp;*/
}

float humiget(){
  float humi = dht.readHumidity();

  if (isnan(humi)) {
    humi = 0x00;
  }
  return humi;
}
