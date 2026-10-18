#include <Arduino.h>

HardwareSerial CRSF(1);

uint8_t packet[64];
uint8_t packetLen = 0;

uint16_t channels[16] = {0};

unsigned long lastPrint = 0;

void setup() {
    Serial.begin(115200);

    // CRSF UART
    // GPIO16 = RX
    // GPIO17 = TX
    CRSF.begin(420000, SERIAL_8N1, 16, 17);
}

void parseCRSF() {

    // 0x16 = RC Channels Packed
    if (packet[2] != 0x16) {
        return;
    }

    uint32_t bits = 0;
    uint8_t bitCount = 0;
    uint8_t index = 3;

    // 16 channels, 11 bits each
    for (int ch = 0; ch < 16; ch++) {

        while (bitCount < 11) {
            bits |= ((uint32_t)packet[index++]) << bitCount;
            bitCount += 8;
        }

        channels[ch] = bits & 0x07FF;

        bits >>= 11;
        bitCount -= 11;
    }
}

void loop() {

    // Continuously receive CRSF data
    while (CRSF.available()) {

        uint8_t b = CRSF.read();

        // First byte = address
        if (packetLen == 0) {
            packet[packetLen++] = b;
        }

        // Second byte = packet length
        else if (packetLen == 1) {
            packet[packetLen++] = b;
        }

        // Remaining packet data
        else {
            packet[packetLen++] = b;

            // Packet complete
            if (packetLen >= packet[1] + 2) {
                parseCRSF();
                packetLen = 0;
            }

            // Safety against bad packet length
            if (packetLen >= sizeof(packet)) {
                packetLen = 0;
            }
        }
    }

    // Print channels every 100 ms
    if (millis() - lastPrint >= 1000) {
        lastPrint = millis();

        Serial.println("--------------------");

        for (int i = 0; i < 16; i++) {
            Serial.print("CH");
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.println(channels[i]);
        }
    }
}







/* void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int i = 0;
  Serial.println(i); 
  i++;
  
  int channels[25] {};
  
  if(Serial.available()>24){
  for (int j = 0; j<25; j++ ){
    channels[j] = Serial.read();

  }
}

  for (int j = 0; j<25; j++ ){
    Serial.print("this is channel ");
    Serial.println(j);
    Serial.println(channels[j]);

  }


  
  delay(1000);
} */
