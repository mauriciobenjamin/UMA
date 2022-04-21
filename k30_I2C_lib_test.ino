#include <K30_I2C.h>

K30_I2C k30_i2c = K30_I2C(0x68);

int co2 = 0;
int rc  = 1;

void read_co2() {
  // put your main code here, to run repeatedly:
  Serial.print("Reading K30 sensor ...... \n");
  rc = k30_i2c.readCO2(co2);
  if (rc == 0){
    Serial.print("Succesful reading\n");
    Serial.print(co2);
    Serial.print(" ppm");
  } else{
    Serial.print("Failure to read sensor\n");  
  }
  Serial.println("\n"); 
  delay(3000); //time between readings
}

