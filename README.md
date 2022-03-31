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
    - [X] BH1750
    - [X] PMSA0031
    - [X] BME688
    - [X] K30
    - [ ] LOX2
    - [ ] Adalogger
- [ ] Obtener lecturas individuales y monitorearlas en el serial
    - [X] BH1750
    - [ ] PMSA0031
    - [X] BME688
    - [ ] K30
    - [ ] LOX2
    - [ ] Adalogger
- [ ] Comunicar con el modulo de BTL y monitorearlo en serial
- [ ] Obtener lecturas de todos los sensores de forma simultanea y monitorearlas en el serial
- [ ] Guardar los datos registrados en SD en tiempo fijo (5 min)
- [ ] Transmitir los datos por Bluetooth

### Sensores

*BME 688* sensor de humedad ambiental, temperatura, presión atmósferica y VOCs en el breakout de [Adafruit](https://learn.adafruit.com/adafruit-bme680-humidity-temperature-barometic-pressure-voc-gas/overview). Ya que el sensor se lee por SPI los pines a configurar son:
```arduino
#define BME_SCK 24
#define BME_MISO 22
#define BME_MOSI 23
#define BME_CS A5
```

*PMSA003I sensor de calidad del aire* en versión de breakout de [Adafruit](https://learn.adafruit.com/pmsa003i) para conexión por I2C.

*BH1750 sensor de luz* ambiental en breakout de [Adafruit](https://learn.adafruit.com/adafruit-bh1750-ambient-light-sensor)

*K30 sensor de CO_2_* por interferometría no dispersiva de IR de [SenseAir](https://senseair.com/products/flexibility-counts/k30/)

### Microcontrolador

El control se realiza con un chip ATSAMD21 que integra un modulo de Bluetooth el [Adafruit Feather con BTE](https://learn.adafruit.com/adafruit-feather-m0-bluefruit-le) 
El modulo requiere de configuraciones especiales en la librería de Adafruit, para configurar el modulo SPI de hardware con los siguientes pines CS = 8, IRQ = 7, RST = 4.

### Librerías y configuración

#### K30

El K30 es un sensor muy usado, por lo que cuenta con varias librerías. Comenzamos las pruebas con [esta](https://github.com/FirstCypress/K30_CO2_I2C_Arduino)