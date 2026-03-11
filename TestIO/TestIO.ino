#define MOSI 11//MOSI
#define MISO  12//MISO
#define CLK  13//sck
#define CS 10//ss

uint8_t SPItransfer(uint8_t data){
  uint8_t recived =0;
  digitalWrite(CS,LOW);
  for (int i=7; i>0;i--){
    digitalWrite(MOSI, (data & (1 << i)) ? HIGH : LOW);
    digitalWrite(CLK,HIGH);
    if(digitalRead(MISO)==1 ? recived|=(1<<i):recived|=(0<<i));
    digitalWrite(CLK,LOW);
    
  }
}



void setup() {
  // put your setup code here, to run once:
  pinMode(MOSI,OUTPUT);
  pinMode(CS,OUTPUT);
  pinMode(CLK,OUTPUT);
  pinMode(MISO,INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

}
