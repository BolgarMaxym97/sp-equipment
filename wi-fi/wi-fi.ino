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

RF24 radio(4, 15);    // Set up nRF24L01 radio on SPI bus plus pins 4 for CE and 15 for CSN

const char* ssid = "kyivstar56";
const char* password = "415689313M";  
float humanity, temperature ;

void setup(void){     
  SPI.setHwCs(true);
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);   
  
  radio.begin();                                        
  radio.setChannel(5);
  radio.setDataRate     (RF24_1MBPS);
  radio.setPALevel      (RF24_PA_HIGH);
  radio.openReadingPipe (1, 0x33);
  radio.startListening  ();
  Serial.begin(115200);  
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);  
  while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
  Serial.print(radio.available());
  } 
  Serial.println(WiFi.localIP());      
}
  
void loop(void){
 float data[2];   
 if(radio.available()){
  radio.read(&data, sizeof(data));
  humanity=data[0]; 
  temperature=data[1];  
  Serial.println(temperature);   

  HTTPClient http;
  http.begin("http://api.smart-plants.me/data-fill");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.POST("data="+String(temperature)+"&user_id=1&sensor_id=1&node_id=1&type=temperature");
  String payload = http.getString();
  Serial.println(httpCode);
  Serial.println(payload);
  http.end();
  }
}



 
