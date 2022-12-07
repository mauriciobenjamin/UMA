#include <ArduinoRS485.h>
#include <ArduinoModbus.h>

#define CONTROLLER_BAUD 9600
#define CONTROLLER_SLAVE 42

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Prueba de Modbus");
  // Inicia el servidor de Modbus con la id 42 y 9600 de velocidad
  if(!ModbusRTUServer.begin(CONTROLLER_SLAVE, CONTROLLER_BAUD)) {
    Serial.println("Falló al iniciar el servidor de Modbus");
    while(1);
  }
  //Configura los registros de inputs y su dirección
  ModbusRTUServer.configureInputRegisters(0x00, numInputRegisters);

}

void loop() {
  ModbusRTUServer.poll();
  Serial.println("Esto tampoco conoce de arduino")


}
