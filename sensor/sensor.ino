#include <SPI.h>                                            // Подключаем библиотеку для работы с шиной SPI
#include <nRF24L01.h>                                      // Подключаем файл настроек из библиотеки RF24
#include "RF24.h"                                         // Подключаем библиотеку для работы с nRF24L01+
#include "DHT.h"
#define DHTPIN 2
#include <OneWire.h>
#include <DallasTemperature.h>



#define PIN_DS18B20 2
 RF24   radio(9, 10);  // Создаём объект radio для работы с библиотекой RF24, указывая номера выводов nRF24L01+ (CE, CSN)   
 OneWire oneWire(PIN_DS18B20);
DallasTemperature dallasSensors(&oneWire); 

// Специальный объект для хранения адреса устройства
DeviceAddress sensorAddress;

void setup(){
    Serial.begin(9600);
    radio.begin();                                        // Инициируем работу nRF24L01+
    radio.setChannel(5);                                  // Указываем канал передачи данных (от 0 до 127), 5 - значит передача данных осуществляется на частоте 2,405 ГГц (на одном канале может быть только 1 приёмник и до 6 передатчиков)
    radio.setDataRate     (RF24_1MBPS);                   // Указываем скорость передачи данных (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек
    radio.setPALevel      (RF24_PA_HIGH);                 // Указываем мощность передатчика (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm)
    radio.openWritingPipe (0x33);               // Открываем трубу с идентификатором 0x1234567890 для передачи данных (на ожном канале может быть открыто до 6 разных труб, которые должны отличаться только последним байтом идентификатора)       
    Serial.print("Ищем устройства...");
    dallasSensors.begin();
    Serial.print("Найдено ");
    Serial.print(dallasSensors.getDeviceCount(), DEC);
    Serial.println(" устройств.");

  if (!dallasSensors.getAddress(sensorAddress, 0)) Serial.println("Не можем найти первое устройство");
  dallasSensors.setResolution(sensorAddress, 12);
}

void loop(){
  float data[2];   
  delay(2000);  
  Serial.println("Измеряем температуру...");
  dallasSensors.requestTemperatures();
  float temperature = getTemperature(sensorAddress);
  Serial.println(temperature); 
  data[0] = 1.0; // заполняем массив
  data[1] = temperature;   
  if(radio.write(&data, sizeof(data))) {
    Serial.println("Sended");
  } else {
    Serial.println("Not sended");
  }
  delay(60000 * 20);
  }
  
float getTemperature(DeviceAddress deviceAddress){
  float tempC = dallasSensors.getTempC(deviceAddress);
  return tempC;
}
