#include <SPI.h>
#include <nRF24L01.h>
#include "RF24.h"
#include "DHT.h"

#define DHTPIN 2
#define  gerconPin  3

RF24   radio(9, 10);
DHT dht(DHTPIN, DHT22);
float data[3];
int lowPowerCounter = 0;
unsigned long timing;
void setup(){
    Serial.begin(9600);
    setRadioSettings();
    setSensorsSettings();
}

void loop(){
    if (millis() - timing > 10000) {
      timing = millis();
      collectData();
      sendData();
    }
}

void setRadioSettings() {
    radio.begin();
    radio.setChannel(5);
    radio.setDataRate(RF24_1MBPS);
    radio.setPALevel(RF24_PA_HIGH);
    radio.openWritingPipe(0x33);
}

void setSensorsSettings() {
    dht.begin();
    pinMode(gerconPin, INPUT);
}

void collectData() {
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    byte gercon = digitalRead(gerconPin);
    data[0] = h;
    data[1] = t;
    if (gercon == HIGH) {
      data[2] = 1;
    }
   else {
      data[2] = 0;
    }
}

void sendData() {
  Serial.println(data[0]);
    Serial.println(data[1]);
    Serial.println(data[2]);
    if(radio.write(&data, sizeof(data))) {
        Serial.println("Sended");
    } else {
        Serial.println("Not sended");
    }
}
