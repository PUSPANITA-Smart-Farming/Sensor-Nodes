#include "ThingsBoard.h"
#include <ESP8266WiFi.h>           //Setup for ESP32 <WiFi.h> change to <ESP8266WiFi.h> if you want to use ESP8266(WARNING ESP 8266 has only 1 Analog input!)
//setup ISP
#define WIFI_AP             "Internet"            //PUSPITA AP
#define WIFI_PASSWORD       "Passwort"        //Password
//setup token
#define DESIRED_TOKEN       "node2"           // change this according to desired node (TOKEN)
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
int t = 0;
int h = 0;
int sm = 0;
int lux = 0;

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
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

  //create random data

  t = random(23, 39);
  h = random(45, 85);
  sm = random(40, 90);
  lux = random(500, 1023);
  
  //publish random data
  tb.sendTelemetryInt("temperature", t);
  tb.sendTelemetryFloat("humidity", h);
  tb.sendTelemetryFloat("soil moisture", sm);
  tb.sendTelemetryFloat("lux", lux);

  tb.loop();
  digitalWrite(BUILTIN_LED, LOW);
  delay(200);                      
  digitalWrite(BUILTIN_LED, HIGH);
  delay(2000);
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
