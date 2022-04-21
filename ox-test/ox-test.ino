char strBuf[130];

void setup() {
    Serial.begin(115200);
    Serial1.begin(9600);
    while (!Serial) { delay(10); }
    Serial.println("Serial USB conectado");
    while (!Serial1) { delay(10); }
    Serial.println("Serial1 conectado");
    sprintf(strBuf, "M 1\r\n");
    byte command[] = {0x4D, 0x20, 0x31, 0x0D, 0x0A};
    Serial1.print(command);
}

void loop() {
    char lastChar = 0x01;
    char buffer[50];
    sprintf(strBuf, "O\r\n");
    byte command[] = {0x4F, 0x0D, 0x0A}
    Serial.print(command);
    Serial1.write(command);
    uint8_t idx = 0;
    if(Serial1.available()) {
        while ((lastChar != 0x0A) && (idx < 50))
        {
            if (Serial1.available())
            {
                lastChar = Serial1.read();
                buffer[idx++] = lastChar; 
            }
            
        }
        
    }   
    buffer[idx] = '\0';
    sprintf(strBuf, "Mensaje: %s", buffer);
    Serial.println(strBuf);
    delay(2000);
}
