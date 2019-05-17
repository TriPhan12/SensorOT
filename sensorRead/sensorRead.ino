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
char getData();
void tempget();
float humiget();
void setupUDP();

void loop()
{
  while (!Serial)
  {
    //wait for serial port to connect.
  }
  delay(1000);
  setupUDP();
  while ((tempget() != 0x00 ) && (humiget() != 0x00))
  {
    guiDoam();
    delay(2000);
  }
  
}

void setupUDP(){
  Serial.println("udp open");
  delay(100)
  Serial.println("udp bind :: 1212");
  delay(100);
}

void guiDoam(){  
  float doam = humiget();
  String doamChuoi = "doam_";
  doamChuoi = doamChuoi + doam;
  // doamChuoi = "doam_36.23";
  char chuoiXuat[11];
  doamChuoi.toCharArray(chuoiXuat, 11);
  // Serial.println(chuoiXuat);
  doamChuoi = "";
  sendPacket(chuoiXuat);
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

/*
void sendPacket(char *destIpv6Addr, char *message)
{
//// How-to-use: genaral destination message sending function
//// sendPacket("address in IPv6","message");
//// Examble:
//// sendPacket("fdde:ad00:beef::....","Hello,World!");

  Serial.print("udp send ");
  Serial.print(destIpv6Addr);
  Serial.print(" 1212 "); //print the UDP port, this case uses port 1212
  Serial.println(message);
  //Serial.write(0x03); //0x03 a.k.a "End of text" in UTF-8
}
*/
void sendPacket(char *message)
{
  /*
  Function: send mesage to multicast address at port 1212
  Usage: sendPacket("doam1_60.23")
    */
  Serial.print("udp send ff03:: 1212 ");  
  Serial.println(message);
}

void tempget(){
  float temp = dht.readTemperature();
  if (isnan(temp)){
    temp = 0x00;
  }
  return temp;
}

float humiget(){
  float humi = dht.readHumidity();
  if (isnan(humi)) {
    humi = 0x00;
  }
  return humi;
}
