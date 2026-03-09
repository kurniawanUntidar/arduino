#define HEADER_BYTE 0xAA

// Daftar Command (Harus sama dengan di Lazarus)
#define CMD_CHECK_CONN   0x06
#define CMD_GET_ID       0x01
#define CMD_SET_PINS     0x05
#define CMD_READ_BLOCK   0x03
#define CMD_WRITE_BLOCK  0x04
#define CMD_SCAN         0x07

// Definisikan Pin (Bisa dipindah sesuai kebutuhan konektor)
const int EC_CS   = 2;
const int EC_CLK  = 4;
const int EC_MOSI = 5;
const int EC_MISO = 3;


void scanSPILines() {
  String report = "";
  
  // Tes 1: Cek apakah MISO "floating" atau tidak
  // Jika MISO selalu HIGH atau selalu LOW meskipun CLK dimainkan, berarti ada masalah
  pinMode(EC_MISO, INPUT_PULLUP);
  bool initialMISO = digitalRead(EC_MISO);
  
  // Tes 2: Toggle Clock dan lihat apakah ada perubahan di MISO (Noise test)
  digitalWrite(EC_CS, LOW);
  uint8_t dummy = EC_SPI_Transfer(0x00); 
  digitalWrite(EC_CS, HIGH);
  
  // Analisa sederhana
  if (initialMISO == HIGH && dummy == 0xFF) {
    report = "MISO: Stuck HIGH/Disconnected";
  } else if (initialMISO == LOW && dummy == 0x00) {
    report = "MISO: Stuck LOW/Short to GND";
  } else {
    report = "Lines: Pulsing OK";
  }
  sendResponse(0x07, report); // Gunakan CMD baru 0x07 untuk Scan
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

uint8_t EC_SPI_Transfer(uint8_t data) {
    uint8_t receivedData = 0;
    for (int i = 7; i >= 0; i--) {
        // MOSI
        digitalWrite(EC_MOSI, (data & (1 << i)) ? HIGH : LOW);
        // Clock High
        digitalWrite(EC_CLK, HIGH);   
        // Baca MISO
        if (digitalRead(EC_MISO)) {
            receivedData |= (1 << i);
        }
        // Clock Low
        digitalWrite(EC_CLK, LOW);
    }
    return receivedData;
}


void executeCommand(uint8_t cmd, uint8_t* data, uint8_t len) {
  switch (cmd) {
    case CMD_CHECK_CONN:
      sendResponse(CMD_CHECK_CONN, "Device Connected");
      digitalWrite(13,HIGH);
      break;

    case CMD_GET_ID:
      uint8_t id[3];
      //uint8_t manID;
      digitalWrite(EC_CS, LOW);    // Aktifkan Chip
      EC_SPI_Transfer(0x9F);       // Perintah JEDEC ID
      id[0] = EC_SPI_Transfer(0x00); // Manufacturer ID
      id[1] = EC_SPI_Transfer(0x00); // Memory Type
      id[2] = EC_SPI_Transfer(0x00); // Capacity
      digitalWrite(EC_CS, HIGH);   // Matikan Chip
    
      // Kirim hasil ke Lazarus dalam format Hex String
      String hexID = "";
      for(int i=0; i<3; i++) {
          if(id[i] < 0x10) hexID += "0";
          hexID += String(id[i], HEX);
      }
      hexID.toUpperCase();
      sendResponse(CMD_GET_ID, hexID);
//      sendResponse(CMD_GET_ID, manID);
//      sendResponse(CMD_GET_ID, "Read ID");
      // Panggil fungsi SPI Bit-Banging yang kita buat sebelumnya
      // Kirim balik ID chip ke Lazarus
      break;
      
    case CMD_WRITE_BLOCK:
      // Tulis data dari buffer ke Chip EC
      sendResponse(0x04, "Write OK");
      break;

    case CMD_SCAN:
      scanSPILines();
      break;
  }
}



void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT); // CS
  pinMode(3, OUTPUT); // CLK
  pinMode(4, OUTPUT); // MOSI
  pinMode(5, INPUT);  // MISO
  pinMode(13,OUTPUT); //connect indicator
  pinMode(12,OUTPUT); //run indicator

}

void loop() {

 if (Serial.available() > 3) { // Minimal Header + Cmd + Len
    if (Serial.read() == HEADER_BYTE) {
      //Serial.write(HEADER_BYTE);
      uint8_t cmd = Serial.read();//Serial.write(cmd);
      uint8_t len = Serial.read();//Serial.write(len);
      uint8_t buffer[256];
      uint8_t checksum = HEADER_BYTE ^ cmd ^ len;

      
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
