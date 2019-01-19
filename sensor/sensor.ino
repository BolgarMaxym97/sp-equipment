#include <SPI.h>
#include <nRF24L01.h>
#include "RF24.h"
#include "DHT.h"
#define DHTPIN 2
#include <OneWire.h>
#include <DallasTemperature.h>

#define PIN_DS18B20 2
#define TIMEOUT 60000 * 20

RF24   radio(9, 10);
OneWire oneWire(PIN_DS18B20);
DallasTemperature dallasSensors(&oneWire);
DeviceAddress sensorAddress;

void setup(){
    Serial.begin(9600);
    setRadioSettings();
    setSensorsSettings();
}
    

void loop(){
    float data[2];
    collectData(data);
    sendDataToHub(data);
    delay(TIMEOUT);
}

float getTemperature(DeviceAddress deviceAddress){
    float tempC = dallasSensors.getTempC(deviceAddress);
    return tempC;
}
    
void setRadioSettings(){
    radio.begin();
    radio.setChannel(5);
    radio.setDataRate(RF24_1MBPS);
    radio.setPALevel(RF24_PA_HIGH);
    radio.openWritingPipe (0x33);
}
    
void setSensorsSettings(){
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

void sendDataToHub(float* data){
    if(radio.write(&data, sizeof(data))) {
        Serial.println("Sended");
    } else {
        Serial.println("Not sended");
    }
}

void collectData(float* data) {
    Serial.println("Измеряем температуру...");
    dallasSensors.requestTemperatures();
    float temperature = getTemperature(sensorAddress);
    Serial.println(temperature);
    data[0] = 1.0;
    data[1] = temperature;
}
