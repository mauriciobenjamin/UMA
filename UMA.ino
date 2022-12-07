/***************************************************************************
 * Sketch para controlar el envió de datos de los sensores por BLE

 Sensores de la UMA
  BH1750 sensor de luz
  BME688 sensor de humedad/presion/temperatura/gas
  LOx sensor de óxigeno

  NOTAS
  El sensor de CO2 queda pendiente de activación
 * 
 * TODO
 * [ ] Programar tiempos de medida
 * [ ] Habilitar modo de deep sleep
 * [ ] Habilitar configuración por el dispositivo central
 * [ ] Habilitar el guardado de datos en la tarjeta SD
 * [ ] Habilitar sensor de CO2
 * [ ] Habilitar sensor de PM2.5
 * [ ] Habilitar la comunicación por RS486
 * [ ] Agregar el envió de datos por Modbus RS485

 * TODO para BLE
 * [ ] Habilitar el modulo de BLE
 * [ ] Enviar los datos de un sensor por BLE
 * [ ] Enviar los datos de todos los sensores en modo de emisión
 * [ ] Enviar los datos de los sensores a demanda
 ***************************************************************************/

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
//Sensor de humedad relativa, temperatura, presión y COVs
#include "Adafruit_BME680.h"
//Sensor de luz
#include "hp_BH1750.h"
//Sensor de PM2.5
#include "Adafruit_PM25AQI.h"
//Libreria de watchdog
#include <Adafruit_SleepyDog.h>
//Software serial para usar el sensor de oxigeno LOx
#include <SoftwareSerial.h>

#define BME_CS A5
#define AQI_SET 13
#define RXPIN 27
#define TXPIN 30

Adafruit_BME680 bme(BME_CS);
hp_BH1750 lux;
Adafruit_PM25AQI aqi = Adafruit_PM25AQI();
SoftwareSerial o2Serial = SoftwareSerial(RXPIN, TXPIN);

void setup () {
  Serial.begin(9600);
  while (!Serial) { delay(10); }
  Serial.println("Unidad de monitoreo ambiental");
  Serial.println("Serial funcionando");

// configuración del Sensor BME688 h/t/p/g
  Serial.println("Probando modulo BME688");
  while(!bme.begin()) {
    Serial.println("No se encontró al sensor BME688");
    delay(1000);
  }

//configuración Sensor de luz
  Serial.println("Probando modulo BH1750");
  bool avail = lux.begin(BH1750_TO_GROUND);

//configuración Sensor de partiiculas
  pinMode(AQI_SET, OUTPUT);
  digitalWrite(AQI_SET, HIGH);

  delay(2000);
  Serial.println("Probando modulo PM25");
  if (! aqi.begin_I2C())
  {
    Serial.println("No se encontró el sensor de PM 2.5");
    delay(1000);
  }

//Configuración del sensor de oxigeno
  pinMode(RXPIN, INPUT);
  pinMode(TXPIN, OUTPUT);
  o2Serial.begin(9600);
      //Verificar la documentación de los comandos válidos en https://cdn.shopify.com/s/files/1/0406/7681/files/Manual-Luminox-LOX-02-CO2-Sensor.pdf 
  o2Serial.write("M 1\r\n");


  bme.setTemperatureOversampling(BME680_OS_8X); 
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
  lux.start();
}

void loop() {
  digitalWrite(AQI_SET, HIGH);
  delay(1000*15);

  if (! bme.performReading())
  {
    Serial.println("Fallo la lectura del sensor BME688");
    return;
  }

  PM25_AQI_Data data;
  Serial.print("{\"humedad\":");
  Serial.print(bme.humidity);
  Serial.print(",");

  Serial.print("\"temperatura\": ");
  Serial.print(bme.temperature);
  Serial.print(",");
  
  Serial.print("\"presion\": ");
  Serial.print(bme.pressure / 100.0);
  Serial.print(",");

  Serial.print("\"vocs\": ");
  Serial.print(bme.gas_resistance / 1000.0);

  if (lux.hasValue() == true) 
  {
    float luxValue = lux.getLux();
    Serial.print(",");
    Serial.print("\"luz\": ");
    Serial.print(luxValue);
    lux.start(); 
  }

  if(aqi.read(&data)) {
    Serial.print(",");
    Serial.print("\"particulas\": [");
    Serial.print(data.particles_100um);
    Serial.print(",");
    Serial.print(data.particles_50um);
    Serial.print(",");
    Serial.print(data.particles_25um);
    Serial.print(",");
    Serial.print(data.particles_10um);
    Serial.print(",");
    Serial.print(data.particles_05um);
    Serial.print(",");
    Serial.print(data.particles_03um);
    Serial.print("]");
  }

  Serial.print(",");
//Solicitud de datos del porcentaje de O2
  o2Serial.write("%\r\n");
  u_int response = o2Serial.available();
  char value[10];
  if(response) {
    for(u_int i=0; response > i; i++) {
        value[i] = o2Serial.read();
      }
  }

  String oxigeno = value;

  Serial.print("\"oxigeno\": ");
  Serial.println(oxigeno.substring(3));

  digitalWrite(AQI_SET, LOW);
  // Al dormir provoca errores en la lectura del serial de la RPi
  // int sleepMS;
  // for (u_int i = 0; i < 4; i++)
  // {
  //   sleepMS = Watchdog.sleep(1000*8);
  // }
  delay(1000*5); 
}
