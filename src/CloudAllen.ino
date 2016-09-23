#include <ArduinoJson.h>

/**************************************************************
 * Blynk is a platform with iOS and Android apps to control
 * Arduino, Raspberry Pi and the likes over the Internet.
 * You can easily build graphic interfaces for all your
 * projects by simply dragging and dropping widgets.
 *
 *   Downloads, docs, tutorials: http://www.blynk.cc
 *   Blynk community:            http://community.blynk.cc
 *   Social networks:            http://www.fb.com/blynkapp
 *                               http://twitter.com/blynk_app
 *
 * This example code is in public domain.
 *
 **************************************************************
 * Project setup in the Blynk app:
 *   Gauge widget on V4
 *
 **************************************************************/

#include <ESP8266WiFi.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
const char auth[] = "YourAuthToken";

// Network settings
const char ssid[] = "YourWiFi";
const char pass[] = "YourPassword";

// Blynk cloud server
const char* host = "api.openweathermap.org";
const char* getData = "/data/2.5/weather?id=5345679&APPID=af5f06783c5f9c554b96053c9a5d003b";

unsigned int port = 8080;

WiFiClient client;

StaticJsonBuffer<200> jsonBuffer;

void setup()
{
  Serial.begin(9600);
  delay(10);
  Serial.println();
  Serial.println();

  // Start the WiFi connection
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
}

void loop() {
  Serial.print("Connecting to ");
  Serial.print(host);
  Serial.print(":");
  Serial.print(port);
  Serial.print("... ");
  if (client.connect(host, port)) {
    Serial.println("OK");

    char pin[] = "V2";

    String getReq = String("GET ") + getData;

    Serial.println("Sending GET request");
    client.print(getReq);
    //Serial.println(getReq);

    Serial.println("Waiting response");
    int timeout = millis() + 5000;
    while (client.available() == 0) {
      if (timeout - millis() < 0) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }

    Serial.println("Reading response");
    int contentLength = 0;
    while (client.available()) {
      String line = client.readStringUntil('\n');
      line.trim();
      if (line.startsWith("Content-Length:")) {
        contentLength = line.substring(line.lastIndexOf(':') + 1).toInt();
      } else if (line.length() == 0) {
        break;
      }
    }

    // Read body
    String body;
    while (body.length() < contentLength && client.connected()) {
      while (client.available()) {
        char c = client.read();
        body += c;
      }
    }
    client.stop();
    Serial.println("Done.");

    Serial.print("Read value: ");
    Serial.println(body);

    JsonObject& root = jsonBuffer.parseObject(body);
    if (!root.success()) {
      Serial.println("parseObject() failed");
      break;
    }
    else{
      int id = root["weather"][0]["id"];
      Serial.println(id);
    }

    while (true) {
      delay(0);
    }
  } else {
    Serial.println("failed");
    delay(5000);
  }
}

