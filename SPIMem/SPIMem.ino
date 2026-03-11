#include <SPI.h>

const int CSPin = 10;

uint8_t SPIreadByte(uint16_t addr){
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  digitalWrite(CSPin,LOW);
  uint8_t b=highByte(addr);
  b=b<<3;
  b=b|0x3;
  SPI.transfer(b);
  SPI.transfer(lowByte(addr));
  digitalWrite(CSPin,HIGH);
  SPI.endTransaction();
  return b;  
}

void setup() {
  Serial.begin(115200);
  pinMode(CSPin, OUTPUT);
  digitalWrite(CSPin,HIGH);
  SPI.begin();
}

void loop(){
  char ch[30];
  for(int i=0;i<256;i++){
    if((i&15)==0){
      sprintf(ch,"\n0x%03X: ",i);
      Serial.print(ch);
    }
    sprintf(ch,"0x%02X", SPIreadByte(i));
    Serial.print(ch);
  }
  delay(5000);  
}
