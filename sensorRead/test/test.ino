#include <string.h>
void setup()
{
    Serial.begin(115200);
    char a = "123";
    char b = "456";
    strcpy(a,b);
    Serial.println(a);
}

//Global variable
const int receivePacketInfo = 2; //3 parameters: bytes received; source address; message content (start from 0)
char receiveMessage;
int node = 0; //The number of node in Thread network (start from 0)
//Functions
void receivePacket();
void sendPacket();
void formatPrint();
char getData();
void readPacket();
void loop()
{
    receivePacket("Xin chao toi la qwen");
    Serial.println("Day la packet");
    for (int i = 0; i < 3; i++)
    {
        Serial.println(receiveMessage);
    }
    delay(1000);
    readPacket("xin chao tat ca cac ban.");
}

void receivePacket(char *packet)
{
    /*How-to-use
  receivePacket(packet);
  Examble:
  receivePacket("2 bytes from fdde:ad00:beef:0:cf3c:df09:f013:55a1 14152 Hello,World!");
              ! -> pointer = 0
                           ! -> pointer = 3
                                                                      ! -> pointer = 5
  */
    String stringPacket = String(*packet);
    int pointer = 0;
    for (int i = 0; i <= stringPacket.length(); i++)
    {
        char buffer = "";
        if (!(packet[i] == " "))
        {
            buffer += packet[i]; //when not reach " " symbol add char to buffer
        }
        else if (pointer == 0)
        {
            //receiveMessage[0] = buffer; //assign buffer to byte number parameter
        }
        else if (pointer == 3)
        {
            //receiveMessage[1] = buffer; //assign buffer to address parameter
        }
        else if (pointer == 5)
        {
            //receiveMessage[2] = buffer; //assign buffer to message parameter
        }
        pointer++; //increase pointer to the beginning of a word
    }
    //receiveMessage.toCharArray(mess,2);
}

void readPacket(char *packet)
{
    int pointer = 0;
    //String chuoiDulieu = String(*packet);
    String chuoiTongA = "";
    Serial.print("Chuoi A ban dau: ");
    Serial.println(chuoiTongA);
    for (int i = 0; i < 30; i++)
    {
        if (packet[i] != 'a')
        {
            Serial.println(packet[i]);
            chuoiTongA = chuoiTongA + packet[i];
            Serial.print("Chuoi tong a:");
            Serial.println(chuoiTongA);
        }
    }
}