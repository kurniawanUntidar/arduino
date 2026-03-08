#define HEADER_BYTE 0xAA

// Daftar Command (Harus sama dengan di Lazarus)
#define CMD_CHECK_CONN   0x06
#define CMD_GET_ID       0x01
#define CMD_SET_PINS     0x05
#define CMD_READ_BLOCK   0x03
#define CMD_WRITE_BLOCK  0x04

// Definisikan Pin (Bisa dipindah sesuai kebutuhan konektor)
const int EC_CS   = 2;
const int EC_CLK  = 3;
const int EC_MOSI = 4;
const int EC_MISO = 5;

void executeCommand(uint8_t cmd, uint8_t* data, uint8_t len) {
  switch (cmd) {
    case CMD_CHECK_CONN:
      sendResponse(CMD_CHECK_CONN, "Device Connected");
      break;

    case CMD_GET_ID:
      // Panggil fungsi SPI Bit-Banging yang kita buat sebelumnya
      // Kirim balik ID chip ke Lazarus
      break;
      
    case CMD_WRITE_BLOCK:
      // Tulis data dari buffer ke Chip EC
      sendResponse(0x04, "Write OK");
      break;
  }
}

void sendResponse(uint8_t cmd, String msg) {
//-----------------arduino atmega328/168-------------------------//
  uint8_t len = msg.length();
  uint8_t checksum = HEADER_BYTE ^ cmd ^ len; // Mulai hitung dari header, cmd, dan len
  Serial.write(HEADER_BYTE);
  Serial.write(cmd);
  Serial.write(msg.length());
  //Serial.print(msg);
  for (int i = 0; i < len; i++) {
    Serial.write(msg[i]);
    checksum ^= msg[i]; // XOR setiap karakter dalam string
  }
  Serial.write(checksum); // Kirim byte terakhir sebagai checksum
  // Tambahkan kalkulasi checksum untuk respon jika perlu
}

void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT); // CS
  pinMode(3, OUTPUT); // CLK
  pinMode(4, OUTPUT); // MOSI
  pinMode(5, INPUT);  // MISO

}

void loop() {

 if (Serial.available() > 3) { // Minimal Header + Cmd + Len
    if (Serial.read() == HEADER_BYTE) {
      //Serial.write(HEADER_BYTE);
      uint8_t cmd = Serial.read();//Serial.write(cmd);
      uint8_t len = Serial.read();//Serial.write(len);
      uint8_t buffer[256];
      uint8_t checksum = HEADER_BYTE ^ cmd ^ len;
      
//      for (int i = 0; i < len; i++) {
//        while (!Serial.available()); // Tunggu byte data
//        buffer[i] = Serial.read();
//        checksum ^= buffer[i];
//      }
      
     // while (!Serial.available()){
      uint8_t receivedChecksum = Serial.read();//Serial.write(receivedChecksum );
       if (checksum == receivedChecksum) {
        executeCommand(cmd, buffer, len);
      } else {
        sendResponse(0xEE, "Error Checksum"); // Kode error
      }
    //  }
    }
 }
}
