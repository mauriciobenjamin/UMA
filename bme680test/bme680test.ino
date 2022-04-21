/***************************************************************************
  This is a library for the BME680 gas, humidity, temperature & pressure sensor

  Designed specifically to work with the Adafruit BME680 Breakout
  ----> http://www.adafruit.com/products/3660

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include <K30_CO2.h>
#include <hp_BH1750.h>

#define BME_SCK 24
#define BME_MISO 22
#define BME_MOSI 23
#define BME_CS A5

#define SEALEVELPRESSURE_HPA (1013.25)

#define _PIN_CALIBRATE 40

K30_CO2 k30(Wire, _PIN_CALIBRATE);
hp_BH1750 BH1750;

//Adafruit_BME680 bme; I2C
Adafruit_BME680 bme(BME_CS); // hardware SPI
//Adafruit_BME680 bme(BME_CS, BME_MOSI, BME_MISO,  BME_SCK);


void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println(F("BME680 test"));

  while (!bme.begin()) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    delay(1000);
  }
  Serial.println("Sensor encontrado");

  Wire.begin();

  // Set up the encoder
  k30.init();

  // Set up de light sensor BH1750

  bool avail = BH1750.begin(BH1750_TO_GROUND);
  
  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
}

void loop() {
  if (! bme.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }
  
  k30.update();

  BH1750.start();
  float lux=BH1750.getLux();  

  Serial.print("CO2: ");
  Serial.print(k30.get_CO2());
  Serial.println(" ppp");

  Serial.print("Lux: ");
  Serial.print(lux);
  Serial.println(" lux");

  Serial.print("Temperature = ");
  Serial.print(bme.temperature);
  Serial.println(" *C");

  Serial.print("Pressure = ");
  Serial.print(bme.pressure / 100.0);
  Serial.println(" hPa");

  Serial.print("Humidity = ");
  Serial.print(bme.humidity);
  Serial.println(" %");

  Serial.print("Gas = ");
  Serial.print(bme.gas_resistance / 1000.0);
  Serial.println(" KOhms");

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  Serial.println();
  delay(2000);
}