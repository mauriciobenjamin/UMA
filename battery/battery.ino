#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"
#include "Adafruit_BLEBattery.h"
#include "Adafruit_BLEGatt.h"
#include "BluefruitConfig.h"
#include "Adafruit_BME680.h"
#include "IEEE11073float.h"

#define VBATPIN A7
#define FACTORYRESET_ENABLE 1
#define BME_CS A5

Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

Adafruit_BLEBattery battery(ble);
Adafruit_BLEGatt gatt(ble);

Adafruit_BME680 bme(BME_CS);

void error(const __FlashStringHelper*err) {
    Serial.println(err);
    while (1);
}

union floatBLE
{
    float number;
    uint8_t bytes[4];
};


/* Información del servicio y características*/
int32_t maServiceId;
int32_t humMeasureCharId;
int32_t temMeasureCharId;

void setup() {
    while (! Serial);
    delay(500);
    Serial.begin(115200);
    Serial.println("Ejemplo con bateria y sensor de humedad");

/**********************************************
 * Configuración del BLE
 * ********************************************/

    if (! ble.begin(VERBOSE_MODE))
    {
        error(F("No se pudo encontrar Bluefruit"));
    }
    Serial.println("Modulo encontrado");

    if (FACTORYRESET_ENABLE)
    {
        Serial.println(F("Realizando el borrado de fabrica"));
        if (! ble.factoryReset() )
        {
            error(F("No fue posible reiniciar el segmento"));
        }
        
    }

    ble.echo(false);
    Serial.println("Solicitando la información");
    ble.info(); 

    Serial.println("Modificando el nombre del dispositivo a 'UMAv0.1':");
    if (! ble.sendCommandCheckOK(F("AT+GAPDEVNAME=UMAv0.1")))
    {
        error(F("No se pudo configurar el nombre del dispositivo"));
    }

    /* Servicio de monitoreo ********************/
    Serial.println(F("Agregando la definición del servicio de monitoreo ambiental (UUID = 0x181A): "));
    maServiceId = gatt.addService(0x181A);
    if (maServiceId == 0)
    {
        error(F("No se pudo crear el servicio de monitoreo"));
    }

    /* Humedad relativa ***********************/
    Serial.println(F("Agregando la característica de humedad (UUID = 0x2A6F): "));
    humMeasureCharId = gatt.addCharacteristic(0x2A6F, GATT_CHARS_PROPERTIES_INDICATE, 4, 4, BLE_DATATYPE_BYTEARRAY);
    if (humMeasureCharId == 0)
    {
        error(F("No se pudo crear la característica de humedad"));
    }

    /* Temperatura **************************/
    Serial.println(F("Agregando la característica de temperatura (UUID = 0x2A6E): "));
    temMeasureCharId = gatt.addCharacteristic(0x2A6E, GATT_CHARS_PROPERTIES_INDICATE, 4, 4, BLE_DATATYPE_BYTEARRAY);
    if (temMeasureCharId == 0) 
    {
        error(F("No se pudo agregar la característica de temperatura"));
    }

    Serial.println(F("Se agrega la UUID del servicio a la carga de anuncios: "));
    uint8_t advdata[] { 0x02, 0x01, 0x06, 0x05, 0x02, 0x09, 0x18, 0x0a, 0x18};
    ble.setAdvData(advdata, sizeof(advdata));
    
    /* Se tiene que reiniciar el dispositivo después de hacer cambios en los servicios para que estos tengan efecto*/
    Serial.println(F("Realizando un reinicio de SW: "));
    ble.reset();
    Serial.print("\n");
    battery.begin(true);

    if (! bme.begin())
    {
        Serial.println("No se encontró el sensor BM688");
        delay(1000);
    } else
    {
        bme.setTemperatureOversampling(BME680_OS_8X); 
        bme.setHumidityOversampling(BME680_OS_2X);
        bme.setPressureOversampling(BME680_OS_4X);
        bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
        bme.setGasHeater(320, 150); // 320*C for 150 ms
    }
    
    
}

void loop(void) {
    float measuredvbat = analogRead(VBATPIN);
    measuredvbat *= 2;    // we divided by 2, so multiply back
    measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
    measuredvbat /= 1024; // convert to voltage
    measuredvbat = measuredvbat/4.2 * 100;
    
    if (!bme.performReading())
    {
        Serial.println("Falló la lectura del sensor BM688");
    } else {
        Serial.print("Humedad: ");
        Serial.print(bme.humidity);
        Serial.println(" %");
        check_hex(bme.humidity);
        floatBLE humBle;
        humBle.number = bme.humidity;
        // float2IEEE11073(bme.humidity, humBle+1);
        gatt.setChar(humMeasureCharId, humBle.bytes, 4);
        Serial.print("\n");

        Serial.print("Temperatura: ");
        Serial.print(bme.temperature);
        Serial.println(" °C");
        check_hex(bme.temperature);
        floatBLE tempBle;
        tempBle.number = bme.temperature;
        // float2IEEE11073(bme.temperature, temp_ble+1);
        gatt.setChar(temMeasureCharId, tempBle.bytes, 4);
        Serial.print("\n");
    }   
    
    Serial.print("Carga de la bateria: " ); Serial.println(measuredvbat);
    battery.update(measuredvbat);
    Serial.println("Datos enviados");

    delay(3000);
}

void check_hex(float data) {
    byte * dataPtr = (byte *) &data;
    Serial.print(F("El datos usa "));
    Serial.print(sizeof(data));
    Serial.print(F(" bytes en la memoria que inicia en la dirección 0x"));
    Serial.println((uint32_t) dataPtr, HEX);
    for (size_t i = 0; i < sizeof(data); i++)
    {
        Serial.print(F("Dirección de memoría 0x"));
        Serial.print((uint32_t) (dataPtr+i), HEX);
        Serial.print(F("= 0x"));
        Serial.println(*(dataPtr+i), HEX);
    }
}