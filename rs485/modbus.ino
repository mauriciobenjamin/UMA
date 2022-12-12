#include <ArduinoRS485.h>
#include <ArduinoModbus.h>

/*
Sensores a reportar
1. Humedad
2. Temperatura
3. Presion
4. VOCs
5. Luz
6. Oxigeno
7. Particulas100
8. Particulas50
9. Particulas25
10. Particulas10
11. Particulas5
12. Particulas3

************
PINES default
txPin 14
rxPin A6
drPin A5
RS485.setPines(int txPin, int drPin, int rxPin);
*/

const int numHoldingRegisters = 12;
const int txPin = 15;
const int drPin = 7;
const int rxPin = 11;

RS485.setPines(txPin, drPin, rxPin);

void setup() {
  Serial.begin(9600);
  while(!Serial) delay(100);
  Serial.println("Prueba de Modbus con RS485");

  // Inicial el servidor de Modbus con direccion 0x01 y velocidad
  // de 9600 baudios
  if(!ModbusRTUServer.begin(1, 9600)) {
    Serial.println("Fallo el inicio del Modbus");
    while(true);
  }
  ModbusRTUServer.configureHoldingRegisters(0x00, numHoldingRegisters);
}

void loop() {
  ModbusRTUServer.poll();
  for (uint i = 0; i < numHoldingRegisters; i++) {
    long holdingRegisterValue = ModbusRTUServer.holdingRegisterRead(i);
    ModbusRTUServer.inputRegisterWrite(i, holdingRegisterValue);
  }
}
