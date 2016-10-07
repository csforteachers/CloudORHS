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


ESP8266WiFiMulti WiFiMulti;
int areaCode = 5345679;
int control = 0;
const byte SERIAL_SOF = 0xA5;
const byte SERIAL_EOF = 0x5A;
const int OUT_BUF_MAX = 3;        


void setup() {
  USE_SERIAL.begin(9600);
  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println();

  for(uint8_t t = 4; t > 0; t--) {
    USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }

  WiFiMulti.addAP("aaa", "aaa");
  Serial.begin(9600);

}

void loop() {
  changeCity("El Dorado Hills");
  retrieveAndLight();
}

void retrieveAndLight(){ //0 for automatic weather 1 for manual blink
  DynamicJsonBuffer jsonBuffer;
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
  byte msg[1];
  if (i == 800){
    USE_SERIAL.printf("Clear Skies: %d\n",i);
    msg[0]=0x01;
    transmitMessage(msg,1);
    return;  
  }
  else if (i==962){
    USE_SERIAL.printf("Hurricane: %d\n",i);
    msg[0]=0x02;
    transmitMessage(msg,1);
  }
  i=i/100;
  if (i==2){
    USE_SERIAL.printf("Thunderstorm: %d\n",i);
    msg[0]=0x03;
    transmitMessage(msg,1);
  }
  else if (i==3){
    USE_SERIAL.printf("Drizzle: %d\n",i);
    msg[0]=0x04;
    transmitMessage(msg,1);
  }
  else if (i==5){
    USE_SERIAL.printf("Rain: %d\n",i);
    msg[0]=0x05;
    transmitMessage(msg,1);
  }
  else if (i==6){
    USE_SERIAL.printf("Snow: %d\n",i);
    msg[0]=0x06;
    transmitMessage(msg,1);
  }
  else if (i==7){
    USE_SERIAL.printf("Crap: %d\n",i);
    msg[0]=0x07;
    transmitMessage(msg,1);
  }
  else if (i==8){
    USE_SERIAL.printf("Clouds: %d\n",i);
    msg[0]=0x08;
    transmitMessage(msg,1);
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
void transmitMessage(byte msg[], uint8_t len) {
  
  int i;
  byte cs;
  byte *out_buf;
  uint8_t buf_size;
  
  // If message is greater than max size, only xmit max bytes
  if ( len > OUT_BUF_MAX ) {
    len = OUT_BUF_MAX;
  }
  
  // Full buffer is message + SOF, EOF bytes
  buf_size = len + 2;
  
  // Create the output buffer with BEGIN, SOF, CS, and EOF
  out_buf = (byte*)malloc(buf_size * sizeof(byte));
  out_buf[0] = SERIAL_SOF;
  memcpy(out_buf + 1, msg, len);
  out_buf[buf_size - 1] = SERIAL_EOF;
  
  // Transmit buffer
  for ( i = 0; i < buf_size; i++ ) {
    Serial.write(out_buf[i]);
  }
  
  // Free some memory
  free(out_buf);
}

