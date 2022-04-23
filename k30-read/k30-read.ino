#include "K30_CO2.h"
#define REG 10

K30_CO2 k30(Wire, 40);


void setup()
{
  Serial.begin(115200);
  while (! Serial)
  {
    delay(10);
  }
  Serial.println("Prueba de CO2");
  Wire.begin();
  k30._debug = 1;
  k30.init();

  for (uint8_t h = 0; h < 20; h++)
  {
    k30.pining();
    Serial.print("\n");
  }
}

void loop()
{
  delay(1000);
  Serial.println("Se apaga al regulador");
  digitalWrite(REG, LOW);

  delay(5000);
  Serial.println("Se reinicia el regulador");
  digitalWrite(REG, HIGH);
  delay(2000);

  Serial.println("Intentando leer el sensor");
  k30.update();

  char buf[50]; sprintf(buf, "CO2: %hu [ ppm ]\n", k30.get_CO2());
  Serial.print(buf);
  delay(3000);
}