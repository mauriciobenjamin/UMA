#include <Arduino.h>
#include <hp_BH1750.h>

hp_BH1750 BH1750;

void setup() {
    Serial.begin(9600);
    //Iniciar el sensor con un pin de dirección conectado a tierra como falso de lo contrario puede que no detecte el sensor
    bool avail = BH1750.begin(BH1750_TO_GROUND); 
}

void loop() {
    BH1750.start();
    float lux = BH1750.getLux();
    Serial.println(lux);
}