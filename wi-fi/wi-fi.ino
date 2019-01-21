//nRF24L01 - ESP8266 12
//1 GRND
//2 VCC
//3 CE   - GPIO4
//4 CSN  - GPIO15
//5 SCK  - GPIO14
//6 MOSI - GPIO13
//7 MISO - GPIO12
//8 IRQ  - NC
 
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <string.h> 
#include <Hash.h>   

RF24 radio(4, 15);

const char* ssid = "kyivstar56";
const char* password = "415689313M";  
float humanity, temperature ;
float data[2];

void setup(void){     
    Serial.begin(115200);
    setSpiSettings();
    setRadioSettings();
    setWiFiSettings();
}
  
void loop(void){ 
    if(radio.available()){
        collectData();
        sendDataToServer();
    }
}

void setSpiSettings(){
    SPI.setHwCs(true);
    SPI.begin();
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
}
void setRadioSettings(){
    radio.begin();
    radio.setChannel(5);
    radio.setDataRate(RF24_1MBPS);
    radio.setPALevel(RF24_PA_HIGH);
    radio.openReadingPipe(1, 0x33);
    radio.startListening();
    Serial.print(radio.available());
}
void setWiFiSettings(){
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println(WiFi.localIP());
}

void collectData() {
    radio.read(&data, sizeof(data));
    humanity=data[0]; 
    temperature=data[1];  
    Serial.println(temperature);   
    Serial.println(humanity);   
}

void sendDataToServer() {
    HTTPClient http;
    http.begin("http://api.smart-plants.me/data-fill");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode = http.POST("data={\"temperature\": "+String(temperature)+", \"humanity\": "+String(humanity)+"}&user_id=1&sensor_id=1&node_id=1");
    String payload = http.getString();
    Serial.println(httpCode);
    Serial.println(payload);
    http.end();
}
