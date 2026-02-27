#define HEADER_BYTE 0xAA

// Daftar Command (Harus sama dengan di Lazarus)
#define CMD_CHECK_CONN   0x06
#define CMD_GET_ID       0x01
#define CMD_SET_PINS     0x05
#define CMD_READ_BLOCK   0x03
#define CMD_WRITE_BLOCK  0x04

// Definisikan Pin (Bisa dipindah sesuai kebutuhan konektor)
const int EC_CS   = 10;
const int EC_CLK  = 13;
const int EC_MOSI = 11;
const int EC_MISO = 12;

void setup() {
  Serial.begin(115200);
  pinMode(10, OUTPUT); // CS
  pinMode(13, OUTPUT); // CLK
  pinMode(11, OUTPUT); // MOSI
  pinMode(12, INPUT);  // MISO

}

void loop() {
  if(Serial.available()>=1){
    uint8_t data = Serial.read();
    Serial.println(data);
  }

}
