/**
 *  CloudAllen.ino
 *
 *  Created on: 21.09.2016
 *  Author: Vasanth Sadhasivan
 */

#include <ArduinoJson.h>


#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;
int greenPin = 14;
int bluePin = 4;
int redPin = 5;
void setup() {

    pinMode(greenPin,OUTPUT);
    pinMode(bluePin,OUTPUT);
    pinMode(redPin,OUTPUT);
    USE_SERIAL.begin(115200);
    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    WiFiMulti.addAP("HOME-XYZ", "1st2oosxturbo1onissan");

}
DynamicJsonBuffer jsonBuffer;
void loop() {
    // wait for WiFi connection
    if((WiFiMulti.run() == WL_CONNECTED)) {
        HTTPClient http;
        USE_SERIAL.print("[HTTP] begin...\n");
        
        http.begin("http://api.openweathermap.org/data/2.5/weather?id=5345679&APPID=af5f06783c5f9c554b96053c9a5d003b"); //HTTP
        //http.begin("http://api.openweathermap.org/data/2.5/weather?id=1488167&APPID=af5f06783c5f9c554b96053c9a5d003b"); //HTTP
        
        USE_SERIAL.print("[HTTP] GET...\n");
        int httpCode = http.GET();
        if(httpCode > 0) {
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);
            if(httpCode == HTTP_CODE_OK) {
                //////////////////THE_GOOD_STUFF/////////////////
                String payload = http.getString(); //DATA WOOOOT
                USE_SERIAL.println(payload);
                JsonObject& root = jsonBuffer.parseObject(payload);
                if (!root.success()) {
                  USE_SERIAL.println("parseObject() failed");    
                }
                else{
                  int id = root["weather"][0]["id"];
                  USE_SERIAL.println(id);
                  parseWeatherConditionID(id);
                }
                //////////////////THE_GOOD_STUFF/////////////////
            }
        } else {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
        http.end();
    }
    delay(10000);
}

void parseWeatherConditionID(int i){
  digitalWrite(bluePin,LOW);
  digitalWrite(greenPin,LOW);
  digitalWrite(redPin,LOW);

  if (i == 800){
    USE_SERIAL.printf("Clear Skies: %d\n",i);
    digitalWrite(bluePin,HIGH);
    return;  
  }
  else if (i==962){
    USE_SERIAL.printf("Hurricane: %d\n",i);
    digitalWrite(redPin,HIGH);
  }
  i=i/100;
  if (i==2){
    USE_SERIAL.printf("Thunderstorm: %d\n",i);
    digitalWrite(greenPin,HIGH);
  }
  else if (i==3){
    USE_SERIAL.printf("Drizzle: %d\n",i);
    digitalWrite(greenPin,HIGH);
  }
  else if (i==5){
    USE_SERIAL.printf("Rain: %d\n",i);
    digitalWrite(greenPin,HIGH);
  }
  else if (i==6){
    USE_SERIAL.printf("Snow: %d\n",i);
    digitalWrite(bluePin,HIGH);
    digitalWrite(greenPin,HIGH);
    digitalWrite(redPin,HIGH);
  }
  else if (i==7){
    USE_SERIAL.printf("Crap: %d\n",i);
    digitalWrite(redPin,HIGH);
  }
  else if (i==8){
    USE_SERIAL.printf("Clouds: %d\n",i);
    digitalWrite(bluePin,HIGH);

  }

}


