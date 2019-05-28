#include "DHT.h"

// Setup buffer size for receiving data
#define CMDBUFFER_SIZE 120

//Setup for the temperature sensor DHT11
#define tempProbe 2
#define DHTTYPE DHT11
DHT dht(tempProbe, DHTTYPE);

// Setup for ground floor light control pin
#define denTangtret 8

unsigned long delayTime = 20000; //20 seconds delay
unsigned long delayStart = 0;    // the time the delay started
bool delayRunning = false;       // true if still waiting for delay to finish

// Define function using in the program

// void setupUDP();
// void serialEvent();
// char processCharInput();
// char processReceiveMessage();
// String getStringPart();
// void guiNhietdo();
// void guiDoam();
// void sendPacket();
// float tempget();
// float humiget();

void setup()
{
  // Set delay for sending data to Thread Network
  delayStart = millis(); // start delay
  delayRunning = true;   // not finished yet
  Serial.begin(115200);
  dht.begin();
  pinMode(denTangtret, OUTPUT); // set pin to output
  delay(1000);
  setupUDP();
}

void loop()
{
  while (!Serial)
  {
    //wait for serial port to connect.
  }
  // check if delay has timed out after 10sec == 10000ms
  if (delayRunning && ((millis() - delayStart) >= delayTime))
  {
    if ((tempget() != 0x00) && (humiget() != 0x00))
    {
      guiDoam();
      guiNhietdo();
    }
    delayStart = millis();
  }
}

/*----------------------------------------------
---------SET UP MODULE CC2538 FUNCTIONS---------
-----------------------------------------------*/

void setupUDP()
{
  Serial.println("udp open");
  delay(100);
  Serial.println("udp bind :: 1212");
  delay(100);
}

/*----------------------------------------------
-------RECEIVE AND PROCESSING ARRIVAL DATA------
-----------------------------------------------*/

void serialEvent()
{
  static char cmdBuffer[CMDBUFFER_SIZE] = "";
  String packet, data;
  char c, dataChuoi[30];
  while (Serial.available())
  {
    c = processCharInput(cmdBuffer, Serial.read());
    // Serial.print(c);
    if (c == '\n')
    {
      // Serial.println();
      //Full command received. Do your stuff here!
      packet = String(cmdBuffer);
      char space = ' ';
      data = getStringPart(packet, space, 5);
      // Serial.print("Data nhan duoc: ");
      // Serial.println(data);
      // Serial.println();
      data.toCharArray(dataChuoi, 30);
      if (strcmp("groundfloor/light_ON", dataChuoi) == 0)
      {
        // Serial.println("Da mo den tang tret");
        digitalWrite(denTangtret, HIGH);
      }
      if (strcmp("groundfloor/light_OFF", dataChuoi) == 0)
      {
        // Serial.println("Da tat den tang tret");
        digitalWrite(denTangtret, LOW);
      }

      memset(cmdBuffer, 0, sizeof(cmdBuffer));        // reset all the charactor in cmdBuffer
    }
  }
  delay(1);
}

char processCharInput(char *cmdBuffer, const char c)
{
  //Store the character in the input buffer
  if (c >= 32 && c <= 126) //Ignore control characters and special ascii characters
  {
    if (strlen(cmdBuffer) < CMDBUFFER_SIZE)
    {
      strncat(cmdBuffer, &c, 1); //Add it to the buffer
    }
    else
    {
      return '\n';
    }
  }
  else if ((c == 8 || c == 127) && cmdBuffer[0] != 0) //Backspace
  {

    cmdBuffer[strlen(cmdBuffer) - 1] = 0;
  }

  return c;
}

char processReceiveMessage(char *packet)
{
  /*
  Usage: return the message data embedded in the packet
  */
  int pointer = 5;
  int i = 0;
  char buffer[30];
  while (pointer > 0)
  {
    if (packet[i] == ' ') //if appeare a ' ' in sentence reduce pointer => enter a new word
    {
      pointer = pointer - 1; //decrease pointer by 1 to get to value 0
    }
    i = i + 1; //increase i to go to next charactor
  }
  if (pointer == 0)
  {
    int j = 0;
    while ((i < strlen(packet)))
    {
      buffer[j] = packet[i];
      i++;
      j++;
    }
  }
  return buffer;
}

// splitting a string and return the part index split by separator
String getStringPart(String data, char separator, int index)
{
  int stringData = 0;   //variable to count data part
  String dataPart = ""; //variable to hole the return text

  for (int i = 0; i < data.length(); i++)
  { //Walk through the text one letter at a time
    if (data[i] == separator)
    {
      //Count the number of times separator character appears in the text
      stringData++;
    }
    else if (stringData == index)
    {
      //get the text when separator is the rignt one
      dataPart.concat(data[i]);
    }
    else if (stringData > index)
    {
      //return text and stop if the next separator appears - to save CPU-time
      return dataPart;
      break;
    }
  }
  //return text if this is the last part
  return dataPart;
}

/*----------------------------------------------
-----SEND DATA TO THREAD NETWORK FUNCTIONS------
-----------------------------------------------*/

void guiNhietdo()
{
  float nhietdo = tempget();
  String nhietdoChuoi = "groundfloor/temp_";
  nhietdoChuoi = nhietdoChuoi + nhietdo;
  char chuoiXuat[26];
  nhietdoChuoi.toCharArray(chuoiXuat, 26);
  nhietdoChuoi = "";
  sendPacket(chuoiXuat);
}

void guiDoam()
{
  /*
  Function: send humidity data read from DHT11 sensor to CC2538 via physical UART port
  Usage: guiDoam();
  */
  float doam = humiget();
  String doamChuoi = "groundfloor/humid_";
  doamChuoi = doamChuoi + doam;
  // doamChuoi = "groundfloor/humid_80.50";
  char chuoiXuat[26];
  doamChuoi.toCharArray(chuoiXuat, 26);
  doamChuoi = "";
  sendPacket(chuoiXuat);
}

void sendPacket(char *message)
{
  /*
  Function: send mesage to multicast address at port 1212
  Usage: sendPacket("doam1_60.23")
    */
  Serial.print("udp send ff03::1 1212 ");
  Serial.println(message);
}

/*----------------------------------------------
--DHT11 TEMPERATURE, HUMIDITY SENSOR FUNCTIONS--
-----------------------------------------------*/

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