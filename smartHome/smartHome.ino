#include "DHT.h"
#include <Servo.h>
#include "MQ135.h"

// Setup buffer size for receiving data
#define CMDBUFFER_SIZE 120

//////////////////////////////////////////////////
////// Setup for DHT11 temperature sensor  ///////
//////////////////////////////////////////////////

#define tempGround 11
#define tempFirst 12
#define DHTTYPE DHT11
DHT dht(tempFirst, DHTTYPE);


//////////////////////////////////////////////////
/////// Setup for MQ135 air quality sensor  //////
//////////////////////////////////////////////////
#define PIN_MQ135 A2 //define the analog input pin for MQ135 air quality sensor
MQ135 airSensor = MQ135(PIN_MQ135);

//////////////////////////////////////////////////
///////////// Setup for Servo Moto  //////////////
//////////////////////////////////////////////////s
Servo door;            // create servo object to control a servo
int doorPosition = 0;  // variable to store the servo position
int minPosition = 0;    //Angle the door close
int maxPosition = 150; //Angle the door open
//////////////////////////////////////////////////
///////////  Setup for light control  ////////////
//////////////////////////////////////////////////
// Setup for ground floor and first floor light control pins
#define denTangtret 4
#define denLau1 7

//////////////////////////////////////////////////
/////// Assign sensor using in the network ///////
//////////////////////////////////////////////////

/////////////// For ground floor /////////////////
bool groundfloor_tempAndhumid = false;
bool groundfloor_light = false;
bool groundfloor_door = false;

//////////////// For first floor /////////////////
bool firstfloor_tempAndhumid = true;
bool firstfloor_light = true;
bool firstfloor_air = true;
//////////////////////////////////////////////////

// Assign variables are used for timer in the program
unsigned long delayTime = 20000; //20 seconds delay
unsigned long delayStart = 0;    // the time the delay started
bool delayRunning = false;       // true if still waiting for delay to finish

// Variable for servo motor that control the groundfloor/door

void setup()
{
  // Set delay for sending data to Thread Network
  delayStart = millis(); // start delay
  delayRunning = true;   // not finished yet
  Serial.begin(115200);
  dht.begin();
  door.attach(9); // attaches the servo on pin 9 to control the door
  door.write(0);
  pinMode(denTangtret, OUTPUT); // set pin to denTangtret
  pinMode(denLau1, OUTPUT);     // set pin to denLau1

  delay(500);
  setupUDP();
}
void loop()
{
  while (!Serial)
  {
    //wait for serial port to connect.
  }
  // check if delay has timed out after delayTime
  if ((groundfloor_tempAndhumid) || (firstfloor_tempAndhumid))
  {
    if (delayRunning && ((millis() - delayStart) >= delayTime))
    {
      if ((tempget() != 0x00) && (humiget() != 0x00))
      {
        guiDoam();
        guiNhietdo();
        guiChatluongKhongKhi();
      }
      delayStart = millis();
    }
  }
  // if ()
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
      if (groundfloor_light)
      {
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
      }
      if (groundfloor_door)
      {
        if ((strcmp("groundfloor/door_ON", dataChuoi) == 0) && (doorPosition < (minPosition + 10)))
        {
          // Serial.println("Da mo cua tang tret");
          doorControl("open");
        }
        if ((strcmp("groundfloor/door_OFF", dataChuoi) == 0) && (doorPosition > (maxPosition - 10)))
        {
          // Serial.println("Da dong cua tang tret");
          doorControl("close");
        }
      }
      if (firstfloor_light)
      {
        if (strcmp("firstfloor/light_ON", dataChuoi) == 0)
        {
          // Serial.println("Da mo den tang 1");
          digitalWrite(denLau1, HIGH);
        }
        if (strcmp("firstfloor/light_OFF", dataChuoi) == 0)
        {
          // Serial.println("Da tat den tang 1");
          digitalWrite(denLau1, LOW);
        }
      }
      memset(cmdBuffer, 0, sizeof(cmdBuffer)); // reset all the charactor in cmdBuffer
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
  String nhietdoChuoi = "";
  if (groundfloor_tempAndhumid)
  {
    nhietdoChuoi = "groundfloor/temp_";
  }
  else if (firstfloor_tempAndhumid)
  {
    nhietdoChuoi = "firstfloor/temp_";
  }
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
  String doamChuoi = "";
  if (groundfloor_tempAndhumid)
  {
    doamChuoi = "groundfloor/humid_";
  }
  else if (firstfloor_tempAndhumid)
  {
    doamChuoi = "firstfloor/humid_";
  }
  doamChuoi = doamChuoi + doam;
  // doamChuoi = "groundfloor/humid_80.50";
  char chuoiXuat[26];
  doamChuoi.toCharArray(chuoiXuat, 26);
  doamChuoi = "";
  sendPacket(chuoiXuat);
}

void guiChatluongKhongKhi()
{
  /*
  Function: send air quality data read from MQ135 sensor to CC2538 via physical UART port
  Usage: guiChatluongKhongKhi();
  */
  float ppm = airSensor.getPPM();
  float nhietdo = dht.readTemperature();
  float doam = dht.readHumidity();
  float correctedPPM = airSensor.getCorrectedPPM(nhietdo, doam);

  String airChuoi = "firstfloor/air_";
  airChuoi = airChuoi + correctedPPM;
  char chuoiXuat[26];
  airChuoi.toCharArray(chuoiXuat, 26);
  airChuoi = "";
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

/*----------------------------------------------
-- DOOR CONTROL USING SERVO 9G MOTOR FUNCTION --
-----------------------------------------------*/
void doorControl(char *action)
{
  if (strcmp("open", action) == 0)
  {
    // Open the door
    for (doorPosition = 0; doorPosition <= maxPosition; doorPosition += 1)
    // goes from 0 degrees to 150 degrees
    {                           // in steps of 1 degree
      door.write(doorPosition); // tell servo to go to position in variable 'pos'
      delay(15);                // waits 15ms for the servo to reach the position
    }
  }
  if (strcmp("close", action) == 0)
  {
    // Close the door
    for (doorPosition = 150; doorPosition >= minPosition; doorPosition -= 1)
    // goes from 150 degrees to 0 degrees
    {                           // in steps of 1 degree
      door.write(doorPosition); // tell servo to go to position in variable 'pos'
      delay(15);                // waits 15ms for the servo to reach the position
    }
  }
}