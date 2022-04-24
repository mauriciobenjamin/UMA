#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"
#include "Adafruit_BLEBattery.h"
#include "BluefruitConfig.h"
#include "Adafruit_BME680.h"

#define VBATPIN A7
#define FACTORYRESET_ENABLE 1
#define BME_CS A5

Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

Adafruit_BLEBattery battery(ble);

Adafruit_BME680 bme(BME_CS);

void error(const __FlashStringHelper*err) {
    Serial.println(err);
    while (1);
}

/* Información del servicio */
int32_t maServiceId;
int32_t maMeasureCharId;
int32_t maLocationCharId;

void setup() {
    while (! Serial);
    delay(500);
    Serial.begin(115200);
    Serial.println("Ejemplo con bateria y sensor de humedad");

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

    Serial.println(F("Agregando la definición del servicio de monitoreo ambiental (UUID = 0x181A): "));
    boolean success = ble.sendCommandWithIntReply(F("AT+GATTADDSERVICE=UUID=0x181A"), &maServiceId);
    if (! success)
    {
        error(F("No se pudo crear el servico de monitoreo"));
    }
    Serial.println(F("Agregando la característica de humedad (UUID = 0x2A6F): "));
    success = ble.sendCommandWithIntReply(F("AT+GATTADDCHAR=UUID=0x2A6F, PROPERTIES=0x10, MIN_LEN=2, VALUE=31.2, DESCRIPTION=Humidity sensor"), &maMeasureCharId);
    if (! success)
    {
        error(F("No se pudo crear la característica de humedad"));
    }
    
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
        ble.print(F("AT+GATTCHAR="));
        ble.print(maMeasureCharId);
        ble.print(F(",00-"));
        ble.println(String(bme.humidity, 2));
        if (!ble.waitForOK())
        {
            Serial.println("Se fallo al obtener una respuesta");
        }
        
    }
    
    Serial.print("Carga de la bateria: " ); Serial.println(measuredvbat);
    battery.update(measuredvbat);

    delay(3000);
}
