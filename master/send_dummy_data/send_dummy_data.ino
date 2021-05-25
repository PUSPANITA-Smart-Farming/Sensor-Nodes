#include "ThingsBoard.h"


#include <WiFi.h>           //Setup for ESP32 change to <ESP8266WiFi.h> if you want to use ESP8266(WARNING ESP 8266 has only 1 Analog input!)
//setup ISP
#define WIFI_AP             "Internet"            //PUSPITA AP
#define WIFI_PASSWORD       "Passwort"        //Password
//setup token
#define DESIRED_TOKEN       "node1"           // change this according to desired node (TOKEN)
#define TOKEN               DESIRED_TOKEN           
#define THINGSBOARD_SERVER  "178.128.112.223" //our thingsboard server

// Baud rate for debug serial
#define SERIAL_DEBUG_BAUD   115200

// Initialize ThingsBoard client
WiFiClient espClient;
// Initialize ThingsBoard instance
ThingsBoard tb(espClient);
// the Wifi radio's status
int status = WL_IDLE_STATUS;

void setup() {
  // initialize serial for debugging
  Serial.begin(SERIAL_DEBUG_BAUD);
  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  InitWiFi();
}

void loop() {
  delay(1000);

  if (WiFi.status() != WL_CONNECTED) {
    reconnect();
  }

  if (!tb.connected()) {
    // Connect to the ThingsBoard
    Serial.print("Connecting to: ");
    Serial.print(THINGSBOARD_SERVER);
    Serial.print(" with token ");
    Serial.println(TOKEN);
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
      Serial.println("Failed to connect");
      return;
    }
  }

  Serial.print("Sending data to: ");
  Serial.println(DESIRED_TOKEN);

  // Uploads new telemetry to ThingsBoard using MQTT.
  // See https://thingsboard.io/docs/reference/mqtt-api/#telemetry-upload-api
  // for more details

  tb.sendTelemetryInt("temperature", 22);
  tb.sendTelemetryFloat("humidity", 42.5);
  tb.sendTelemetryFloat("soil moisture", 62);
  tb.sendTelemetryFloat("lux", 645);

  tb.loop();
}

void InitWiFi()
{
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network

  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
}

void reconnect() {
  // Loop until we're reconnected
  status = WiFi.status();
  if ( status != WL_CONNECTED) {
    WiFi.begin(WIFI_AP, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("Connected to AP");
  }
}
