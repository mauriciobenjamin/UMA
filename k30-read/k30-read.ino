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
  k30._debug = 0;
  k30.init();

  // for (uint8_t h = 0; h < 20; h++)
  // {
  //   k30.pining();
  //   Serial.print("\n");
  // }
}

void loop()
{
  Serial.println("Intentando leer el sensor");
  delay(3000);
  k30.update();

  char buf[50]; sprintf(buf, "CO2: %hu [ ppm ]\n", k30.get_CO2());
  Serial.print(buf);
  delay(3000);
}