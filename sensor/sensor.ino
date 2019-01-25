#include <SPI.h>
#include <nRF24L01.h>
#include "RF24.h"
#include "DHT.h"
#define DHTPIN 2
#include <OneWire.h>
#include <DallasTemperature.h>
#include "LowPower.h"

#define PIN_DS18B20 2
#define PIN_HUMANITY A0
// значение полного полива
#define MINVALUESOILMOISTURE 220
// значение критической сухости
#define MAXVALUESOILMOISTURE 900
#define TIMEOUT_COUNT_BY_8_SEC 150

RF24   radio(9, 10);
OneWire oneWire(PIN_DS18B20);
DallasTemperature dallasSensors(&oneWire);
DeviceAddress sensorAddress;
float data[2];
int lowPowerCounter = 0;

void setup(){
    Serial.begin(9600);
    setRadioSettings();
    setSensorsSettings();
}

void loop(){
    if (lowPowerCounter <= TIMEOUT_COUNT_BY_8_SEC) {
        lowPowerCounter++;
        sleep();
    } else {
        collectData();
        sendData();
        lowPowerCounter = 0;
      }
}
  
float getTemperature(DeviceAddress deviceAddress){
    float tempC = dallasSensors.getTempC(deviceAddress);
    return tempC;
}

void setRadioSettings() {
    radio.begin();
    radio.setChannel(5);
    radio.setDataRate(RF24_1MBPS);
    radio.setPALevel(RF24_PA_HIGH);
    radio.openWritingPipe(0x33);
}

void setSensorsSettings() {
    Serial.print("Ищем устройства...");
    dallasSensors.begin();
    Serial.print("Найдено ");
    Serial.print(dallasSensors.getDeviceCount(), DEC);
    Serial.println(" устройств.");
    if (!dallasSensors.getAddress(sensorAddress, 0)) {
        Serial.println("Не можем найти первое устройство");
    }
    dallasSensors.setResolution(sensorAddress, 12);
}

void collectData() {
    Serial.println("Измеряем температуру...");
    dallasSensors.requestTemperatures();
    float temperature = getTemperature(sensorAddress);
    int humanity = map(analogRead(PIN_HUMANITY), 1025, 110, 0, 100);
    Serial.println(temperature);
    Serial.println(humanity);
    data[0] = humanity;
    data[1] = temperature;
}

void sendData() {
    if(radio.write(&data, sizeof(data))) {
        Serial.println("Sended");
    } else {
        Serial.println("Not sended");
    }
}

void sleep() {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
}
