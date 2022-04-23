#include <SPI.h>
#include <SD.h>
#include "RTClib.h"

// Set the pins used
#define cardSelect 4
#define VBATPIN A7

File logfile;
RTC_PCF8523 rtc;

char filename[15];
// blink out an error code
void error(uint8_t errno)
{
  while (1)
  {
    uint8_t i;
    for (i = 0; i < errno; i++)
    {
      digitalWrite(13, HIGH);
      delay(100);
      digitalWrite(13, LOW);
      delay(100);
    }
    for (i = errno; i < 10; i++)
    {
      delay(200);
    }
  }
}

void setup()
{
  pinMode(13, OUTPUT);

  // see if the card is present and can be initialized:
  Serial.begin(115200);
  while (!Serial)
  {
    delay(10);
  }

  Serial.println("Serial funcionando");
  Serial.println("Verificación de la tarjeta SD");
  
  while (!SD.begin(cardSelect))
  {
    Serial.println("No se encontró la tarjeta SD");
    //error(2);
    delay(5000);
  }

  if (! rtc.begin())
  {
    Serial.println("No se encontró el RTC");
    Serial.flush();
  }

  if (! rtc.isrunning()) {
    Serial.println("El RTC no estaba funcionando, se fijara el tiempo del sistema");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  strcpy(filename, "/BAT_00.CSV");

  for (uint8_t i = 0; i < 100; i++)
  {
    filename[5] = '0' + i / 10;
    filename[6] = '0' + i % 10;
    // create if does not exist, do not open existing, write, sync after write
    if (!SD.exists(filename))
    {
      break;
    }
  }

  logfile = SD.open(filename, FILE_WRITE);

  if (!logfile)
  {
    Serial.print("No se pudo crear el archivo de registro");
    Serial.println(filename);
    error(3);
  }
  Serial.print("Escribiendo en ");
  Serial.println(filename);
  logfile.println("tiempo,voltaje,porcentaje");

  pinMode(13, OUTPUT);
  pinMode(8, OUTPUT);
  logfile.close();
  Serial.println("Listo");
}

void loop()
{
  DateTime time = rtc.now();
  float vbat = analogRead(VBATPIN);
  vbat = vbat * 2 * 3.3 / 1024;
  logfile = SD.open(filename, FILE_WRITE);
  digitalWrite(8, HIGH);
  String datos;
  datos += time.timestamp(DateTime::TIMESTAMP_FULL);
  datos += ", "; datos += vbat; datos += ", ";
  datos += vbat / 4.27 * 100; datos += "%";
  Serial.print(time.timestamp(DateTime::TIMESTAMP_FULL));
  Serial.print(" Bateria = ");
  Serial.println(vbat);
  Serial.println(datos);
  logfile.println(datos);
  logfile.close();
  digitalWrite(8, LOW);

  delay(5000);
}