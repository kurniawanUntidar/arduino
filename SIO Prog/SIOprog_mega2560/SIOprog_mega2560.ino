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
    }
        // Clock Low
        digitalWrite(EC_CLK, LOW);
    
    return receivedData;
}

void executeCommand(uint8_t cmd, uint8_t* data, uint8_t len) {
  switch (cmd) {
    case CMD_CHECK_CONN:
      sendResponse(CMD_CHECK_CONN, "Mega Ready");
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
  Serial.write(HEADER_BYTE);
  Serial.write(cmd);
  Serial.write(msg.length());
  Serial.print(msg);
  // Tambahkan kalkulasi checksum untuk respon jika perlu
}


void setup() {
  Serial.begin(115200);
  pinMode(10, OUTPUT); // CS
  pinMode(13, OUTPUT); // CLK
  pinMode(11, OUTPUT); // MOSI
  pinMode(12, INPUT);  // MISO
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() >= 3) { // Minimal Header + Cmd + Len
    if (Serial.read() == HEADER_BYTE) {
      uint8_t cmd = Serial.read();
      uint8_t len = Serial.read();
      uint8_t buffer[256];
      uint8_t checksum = HEADER_BYTE ^ cmd ^ len;

      // Tunggu dan baca data berdasarkan 'len'
      for (int i = 0; i < len; i++) {
        while (!Serial.available()); // Tunggu byte data
        buffer[i] = Serial.read();
        checksum ^= buffer[i];
      }

      // Baca Checksum terakhir
      while (!Serial.available()){
      uint8_t receivedChecksum = Serial.read();

      if (checksum == receivedChecksum) {
        executeCommand(cmd, buffer, len);
      } else {
        sendResponse(0xEE, "Error Checksum"); // Kode error
      }
    }
}
  }
}
