#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
//Sensor de humedad relativa, temperatura, presión y COVs
#include "Adafruit_BME680.h"
//Reloj de tiempo real
#include "RTClib.h"
//Sensor de luz
#include "hp_BH1750.h"

#define BME_CS A5

Adafruit_BME680 bme(BME_CS);
RTC_PCF8523 rtc;
hp_BH1750 lux;

void setup () {
  Serial.begin(57600);

  while(!bme.begin()) {
    Serial.println("No se encontró al sensor BME688");
    delay(1000);
  }

  bool avail = lux.begin(BH1750_TO_GROUND);

  if (! rtc.begin()) {
    Serial.println("No se pudo encontrar el Reloj de Tiempo Real");
    Serial.flush();
    while (1) delay(10);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

  bme.setTemperatureOversampling(BME680_OS_8X); 
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
  lux.start();
}

void loop() {
  if (! bme.performReading())
  {
    Serial.println("Fallo la lectura :(");
    return;
  }

  if (lux.hasValue() == true) 
  {
    float luxValue = lux.getLux();
    Serial.print("Luz: ");
    Serial.print(luxValue);
    Serial.println(" lumenes.");
    lux.start(); 
  }

  DateTime time = rtc.now();

  //Full Timestamp
  Serial.println(String("Marca de tiempo\t")+time.timestamp(DateTime::TIMESTAMP_FULL));

  Serial.print("Humedad:\t");
  Serial.print(bme.humidity);
  Serial.println(" %");

  Serial.print("Temperatura: ");
  Serial.print(bme.temperature);
  Serial.println(" °C");
  
  Serial.print("Presión: ");
  Serial.print(bme.pressure / 100.0);
  Serial.println(" hPa");
  Serial.println("\n");

  //Delay 5s
  delay(5000);
}