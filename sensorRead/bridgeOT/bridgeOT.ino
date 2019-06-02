#include <SoftwareSerial.h>

// software serial #1: RX = digital pin 10, TX = digital pin 11
SoftwareSerial portOne(10, 11);

void setup()
{
  // Start software serial port
  Serial.begin(9600); 
  while (!Serial) {
    // wait for serial port to connect. Needed for native USB port only
  }
  portOne.begin(9600);
}

void loop(){
  portOne.listen();          
  char inByte = portOne.read();
  while (portOne.available() > 0) {
    String inputString = "";
    while(InByte == 0x03){
      inputString += inByte;
    }
    Serial.println(inputString);
  }
}