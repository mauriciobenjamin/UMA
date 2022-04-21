#include <Arduino.h>
#include "RTClib.h"
#include "hp_BH1750"

RTC_PCF8523 rtc;

hp_BH1750 BH1750;

void setup() {
    Serial.begin(57600);
    //Iniciar el sensor con un pin de dirección conectado a tierra como falso de lo contrario puede que no detecte el sensor
    bool avail = BH1750.begin(BH1750_TO_GROUND); 
    
    // Configuración del RTC
    if (! rtc.begin()) {
        Serial.println("No se pudo encontrar el Reloj de Tiempo Real");
        Serial.flush();
        while (1) delay(10);
    }

    if (! rtc.initialized() || rtc.lostPower())
    {
        Serial.println("El RTC no se ha inicializado. Hay que establecer la fecha");
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }

    rtc.start();
    // Comandos para el ajuste de la calibración de las desviaciones de la hora.
    // Note: any previous calibration should cancelled prior to any new observation period.
    // Example - RTC gaining 43 seconds in 1 week
    float drift = 43; // seconds plus or minus over oservation period - set to 0 to cancel previous calibration.
    float period_sec = (7 * 86400);  // total obsevation period in seconds (86400 = seconds in 1 day:  7 days = (7 * 86400) seconds )
    float deviation_ppm = (drift / period_sec * 1000000); //  deviation in parts per million (μs)
    float drift_unit = 4.34; // use with offset mode PCF8523_TwoHours
    // float drift_unit = 4.069; //For corrections every min the drift_unit is 4.069 ppm (use with offset mode PCF8523_OneMinute)
    int offset = round(deviation_ppm / drift_unit);
    // rtc.calibrate(PCF8523_TwoHours, offset); // Un-comment to perform calibration once drift (seconds) and observation period (seconds) are correct
    // rtc.calibrate(PCF8523_TwoHours, 0); // Un-comment to cancel previous calibration

}

void loop() {
    BH1750.start();
    float lux = BH1750.getLux();
    DateTime now = rtc.now();
    Serial.print(now.timestamp(DateTime::TIMESTAMP_FULL));
    Serial.print(" Luz: ");
    Serial.println(lux);
    delay(5000);
}