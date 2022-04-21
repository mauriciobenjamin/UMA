#include <Adafruit_I2CDevice.h>

#define I2C_ADDRESS 0x68
Adafruit_I2CDevice i2c_dev = Adafruit_I2CDevice(I2C_ADDRESS);

void setup() {
  while (!Serial) { delay(10); }
  Serial.begin(115200);
  Serial.println("I2C device read and write test");

  if (!i2c_dev.begin()) {
    Serial.print("Did not find device at 0x");
    Serial.println(i2c_dev.address(), HEX);
    while (1);
  }
  Serial.print("Device found on address 0x");
  Serial.println(i2c_dev.address(), HEX);
}

void loop() {
  uint8_t buffer_inst[5] = {0xD0, 0x22, 0x00, 0x08, 0x2A};
  uint8_t buffer_read[5];
  // Try to write 4 bytes  
  Serial.print("Write: ");
  i2c_dev.write_then_read(buffer_inst, 5, buffer_read, 5, true);

  for (uint8_t i=0; i<5; i++){
    Serial.print("0x");
    Serial.print(buffer_read[i], HEX);
    Serial.print(", ");
  }
  Serial.println();

  int32_t co2_value = 0;
  co2_value |= buffer_read[1];
  co2_value = co2_value << 8;
  co2_value |= buffer_read[2];

  uint8_t sum = 0;
  sum = buffer_read[0] + buffer_read[1] + buffer_read[2];
  if(sum == buffer_read[3]) {
    Serial.println(co2_value);
  } else {
    Serial.println(-1);
  }
  delay(2000);
}