#include <DHT.h>
#include <NeoHWSerial.h>

//Setup for the temperature sensor DHT11
#define tempProbe 2
#define DHTTYPE DHT11
DHT dht(tempProbe, DHTTYPE);

//Global variable

String receiveMessage = "";
// String sourceAddress = "";
int receiveByte = 0;
String receiveMessageData;
static volatile uint16_t count = 0;

//Functions
void guiDoam();
void guiNhietdo();
String receivePacket();
void sendPacket();
char getData();
float tempget();
float humiget();
void setupUDP();
static void char_received();
String getMessage();
String getMessageData();

void setup()
{
  NeoSerial.begin(115200);
  NeoSerial.attachInterrupt(char_received);
  dht.begin();
}

void loop()
{
  while (!NeoSerial)
  {
    //wait for serial port to connect.
  }
  delay(1000);
  setupUDP();
  if ((tempget() != 0x00) && (humiget() != 0x00))
  {
    guiDoam();
    guiNhietdo();
    delay(100);
  }

  NeoSerial.println(getMessageData());
}

String getMessageData(){
  /*
  Function: Return the received broadcasting message in Thread network
  */ 
  char chuoiMessage;
  String message = getMessage();
  message.toCharArray(chuoiMessage, 70);
  String data = processReceiveMessage(chuoiMessage, 3); //3 for getting message data
  NeoSerial.println("-----------------------------");
  NeoSerial.println(data);
  return data;
}

String getMessage()
{
  ///Setting up for ISR (Interrupt Service Routetine)
  uint8_t oldSREG = SREG;
  noInterrupts();
  uint16_t old_count = count;
  count = 0;
  SREG = oldSREG;
  if (old_count)
  {
    NeoSerial.print("\nPayload received: ");
    NeoSerial.println(old_count - 2); //result is added to 2
    NeoSerial.print("Message receive: ");
    NeoSerial.println(receiveMessage);
  }
  else
  {
    NeoSerial.flush();
    receiveMessage = "";
  }
  return receiveMessage;
}

static void char_received(uint8_t c)
{
  // This is a little naughty, as it will try to block
  //   in this ISR if the tx_buffer is full.  For this example,
  //   we are only sending as many characters as we have received,
  //   and they arrive at the same rate we are sending them.
  char raw = (char)c;
  receiveMessage += raw;
  // NeoSerial.print(raw);
  // NeoSerial.write(c);
  count++;
}

void setupUDP()
{
  NeoSerial.println("udp open");
  delay(100);
  NeoSerial.println("udp bind :: 1212");
  delay(100);
}

void guiDoam()
{
  /*
  Function: send humidity data read from DHT11 sensor to CC2538 via physical UART port
  Usage: guiDoam();
  */
  float doam = humiget();
  String doamChuoi = "doam1_";
  doamChuoi = doamChuoi + doam;
  // doamChuoi = "doam_36.23";
  char chuoiXuat[12];
  doamChuoi.toCharArray(chuoiXuat, 12);
  // NeoSerial.println(chuoiXuat);
  doamChuoi = "";
  sendPacket(chuoiXuat);
}

void guiNhietdo()
{
  float nhietdo = tempget();
  String nhietdoChuoi = "temp_";
  nhietdoChuoi = nhietdoChuoi + nhietdo;
  char chuoiXuat[15];
  nhietdoChuoi.toCharArray(chuoiXuat, 15);
  nhietdoChuoi = "";
  sendPacket(chuoiXuat);
}

String processReceiveMessage(char *packet, int data)
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
    //NeoSerial.println(data);
    pointer = 3;
    break;
  default:
    //NeoSerial.println(data);
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
      //NeoSerial.println(tampont);
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

  NeoSerial.print("udp send ");
  NeoSerial.print(destIpv6Addr);
  NeoSerial.print(" 1212 "); //print the UDP port, this case uses port 1212
  NeoSerial.println(message);
  //NeoSerial.write(0x03); //0x03 a.k.a "End of text" in UTF-8
}
*/
void sendPacket(char *message)
{
  /*
  Function: send mesage to multicast address at port 1212
  Usage: sendPacket("doam1_60.23")
    */
  NeoSerial.print("udp send ff03::1 1212 ");
  NeoSerial.println(message);
}

float tempget()
{
  float temp = dht.readTemperature();
  if (isnan(temp))
  {
    temp = 0x00;
  }
  return temp;
}

float humiget()
{
  float humi = dht.readHumidity();
  if (isnan(humi))
  {
    humi = 0x00;
  }
  return humi;
}
