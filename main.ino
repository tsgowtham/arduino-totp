
#include "sha1.h"
#include "TOTP.h"
#include <DS3231.h>
#include<Keypad.h>


DS3231 rtc(SDA,SCL);
Time t;

//password = abc1234asd
uint8_t hmacKey[] = {0x61, 0x62, 0x63, 0x31, 0x32, 0x33, 0x34, 0x61, 0x73, 0x64};

TOTP totp = TOTP(hmacKey, 10);

char code[7];
char inputCode[7];
int count = 0;

const byte ROWS = 4; 
const byte COLS = 4; 

char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {4, 6, 7, 8}; 
byte colPins[COLS] = {9, 10, 11, 12};  


Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

boolean doorOpen = false;
int attempt = 0;

void alert(int);

void setup() {
  pinMode(2,OUTPUT);
  Serial.begin(115200);
  rtc.begin();
  digitalWrite(2,LOW);
  Serial.println();
  Serial.println("----------------------------------------------------------------------------------------------------------");
  Serial.println("WELCOME HOME");
  Serial.println("----------------------------------------------------------------------------------------------------------");

}
void loop() {

  char inputKey = customKeypad.waitForKey();
  Serial.print(inputKey);
  if(inputKey == '#')
  {
    count = 0;
    Serial.println("\nArray Flushed");
  }
  else if(inputKey == '*')
  {
    count = 0;
    doorOpen = false;
    digitalWrite(2,LOW);
    Serial.println("\nLocked");
  }
  else
  {
     inputCode[count] = inputKey;
     ++count;
     if(count == 6)
     {
       Serial.println();
       long GMT = rtc.getUnixTime(rtc.getTime());
       char* newCode = totp.getCode(GMT);
       inputCode[count] = '\0';
       
       if(strcmp(newCode,inputCode)==0)
       {
         doorOpen = true;
         count = 0;
         Serial.println("LOCK1 opened");
         attempt = 0;
         digitalWrite(2,HIGH);

       }
       else
       {
         Serial.println("Error");
         count = 0;
         ++attempt;
         doorOpen = false;
       }
     }
  }
  if(doorOpen)
    digitalWrite(2,HIGH);
  else
    digitalWrite(2,LOW);
}
