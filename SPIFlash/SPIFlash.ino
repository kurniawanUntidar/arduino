#include <SPIMemory.h>

SPIFlash flash;  // Create flash object

void setup() {
  Serial.begin(115200);

  if (flash.begin()) {
    Serial.println("W25Q Flash Memory Initialized Successfully!");
    Serial.print("JEDEC ID: 0x");
    Serial.println(flash.getJEDECID(), HEX);
    Serial.print("Capacity: ");
    Serial.print(flash.getCapacity());
    Serial.println(" bytes");
  } else {
    Serial.println("Flash Memory Initialization Failed!");
  }
}

void loop(){
  
}
