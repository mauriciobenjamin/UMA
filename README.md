# Unidad de Monitoreo Ambiental

La UMA es un sistema desarrollado para el monitoreo ambiental, permitiendo el almacenamiento de los datos generados al mismo tiempo que se transmite de forma inalámbrica para facilitar su acceso. El sistema puede desplegarse de forma autónoma como registrador de datos o como parte de una red de monitoreo con un concentrador a través de Bluetooth LE. UMA permite el monitoreo de las siguientes variables

- Humedad relativa
- Temperatura
- Presión atmosférica
- Concentración de CO_2_ en el aire (sensor K30)
- Concentración de oxígeno en el aire
- Partículas suspendidas PM2.5
- Radiación luminosa
- Compuestos orgánicos volátiles

## Desarrollo

### TODOs

- [ ] Carga de las librerías
- [ ] Obtener lecturas de todos los sensores de forma simultanea y monitorearlas en el serial
- [ ] Guardar los datos registrados en SD en tiempo fijo (5 min)
- [ ] Transmitir los datos por Bluetooth

## Fuentes

### Microcontrolador

M0 en un (Adafruit Feather con BTE)[https://learn.adafruit.com/adafruit-feather-m0-bluefruit-le] 