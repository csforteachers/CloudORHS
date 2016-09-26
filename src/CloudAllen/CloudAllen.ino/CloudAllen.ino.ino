/**
 *  CloudAllen.ino
 *
 *  Created on: 21.09.2016
 *  Author: Vasanth Sadhasivan
 */

#include <ArduinoJson.h>
#include <BlynkSimpleEsp8266.h>

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial
#define BLYNK_PRINT Serial
// Pin definitions
#define LED_PIN           5
#define WEATHER_LED_PIN   V1
#define DISCO_LED_PIN     V5
#define ZERGBA_PIN        V8

ESP8266WiFiMulti WiFiMulti;
int greenPin = 14;
int bluePin = 4;
int redPin = 5;
int areaCode = 5345679;
int control = 0;
DynamicJsonBuffer jsonBuffer;

void setup() {
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
  Serial.begin(9600);
  Blynk.begin("hello", "HOME-XYZ", "1st2oosxturbo1onissan");

}

void loop() {
  Blynk.run();
  retrieveAndLight();
}

void retrieveAndLight(){ //0 for automatic weather 1 for manual blink
  if(control == 0){
      if((WiFiMulti.run() == WL_CONNECTED)) {
        HTTPClient http;
        USE_SERIAL.print("[HTTP] begin...\n");
        String url = "http://api.openweathermap.org/data/2.5/weather?id="+String(areaCode)+"&APPID=af5f06783c5f9c554b96053c9a5d003b";
        http.begin(url); //HTTP        
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
  else if(control == 1){
    
  }
}

void parseWeatherConditionID(int i){
  if (i == 800){
    USE_SERIAL.printf("Clear Skies: %d\n",i);
    return;  
  }
  else if (i==962){
    USE_SERIAL.printf("Hurricane: %d\n",i);
  }
  i=i/100;
  if (i==2){
    USE_SERIAL.printf("Thunderstorm: %d\n",i);
  }
  else if (i==3){
    USE_SERIAL.printf("Drizzle: %d\n",i);
  }
  else if (i==5){
    USE_SERIAL.printf("Rain: %d\n",i);
  }
  else if (i==6){
    USE_SERIAL.printf("Snow: %d\n",i);
  }
  else if (i==7){
    USE_SERIAL.printf("Crap: %d\n",i);
  }
  else if (i==8){
    USE_SERIAL.printf("Clouds: %d\n",i);
  }
}

void changeCity(String cityName){
  if(cityName =="El Dorado Hills")
  {
    areaCode = 5345679;
  }
  else if(cityName =="Folsom")
  {
    areaCode = 5349705;
  } 
  else if(cityName =="New York")
  {
    areaCode = 5128581;
  } 
  else if(cityName =="Tokyo")
  {
    areaCode = 1850147;
  } 
  else if(cityName =="Paris")
  {
    areaCode = 2988507;
  } 
  else if(cityName =="Hong Kong")
  {
    areaCode = 1819729;
  } 
  else if(cityName =="Hyderabad")
  {
    areaCode = 1269843;
  } 
  else if(cityName =="Los Angeles")
  {
    areaCode = 5368361;
  } 
  else if(cityName =="San Diego")
  {
    areaCode = 5391811;
  } 
  else if(cityName =="Washington")
  {
    areaCode = 4140963;
  } 
  else if(cityName =="London")
  {
    areaCode = 2643741;
  } 
}
BLYNK_WRITE(ZERGBA_PIN)
{
  control=1;
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("Zebra value is: ");
  Serial.println(pinValue);
}
BLYNK_WRITE(DISCO_LED_PIN)
{
  control=1;
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("Disco value is: ");
  Serial.println(pinValue);
}
BLYNK_WRITE(WEATHER_LED_PIN)
{
  control=0;
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("Manual value is: ");
  Serial.println(pinValue);
}

