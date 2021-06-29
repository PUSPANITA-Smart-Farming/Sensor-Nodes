//Internet and ESP Setup
#include "ThingsBoard.h"
#include <WiFi.h>           //Setup for ESP32 change to <ESP8266WiFi.h> if you want to use ESP8266(WARNING ESP 8266 has only 1 Analog input!)

//setup ISP
#define WIFI_AP             "*****"            //PUSPANITA AP
#define WIFI_PASSWORD       "*****"            //Password

//setup token
#define DESIRED_TOKEN       "node1"           // change this according to desired node (TOKEN)
#define TOKEN               DESIRED_TOKEN
#define THINGSBOARD_SERVER  "smart-farming.puspanita.id" //our thingsboard server


// Baud rate for debug serial
#define SERIAL_DEBUG_BAUD 115200

// Initialize ThingsBoard client
WiFiClient espClient;
// Initialize ThingsBoard instance
ThingsBoard tb(espClient);
// the Wifi radio's status
int status = WL_IDLE_STATUS;

//Sensors Code
//BH1750 Lux Sensor SCL to D22, SDA to D21
#include <Wire.h>
#include <SPI.h>
#include <BH1750.h>
BH1750 lightMeter;

//DHT11 Air Humidity and Temperature Sensor
#include "DHT.h"
#define DHTPIN 2                              //Set to pin D2 
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE, 15);

//Soil Moisture Sensor 
#define SoilSensor 36                        //set GPIO 36 (ADC1_0)
const int AirValue = 750;
const int WaterValue = 335;
int soilMoistureValue = 0;
int soilmoisturepercent = 0;

void setup() {
  // initialize serial for debugging
  Serial.begin(SERIAL_DEBUG_BAUD);
  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  InitWiFi();

//Sensors Initializing
  Wire.begin();
  lightMeter.begin();
  dht.begin();
}


void loop() {
//WiFi and Thingsboard Connection
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

//Sensor Data Read and Upload
  soilMoistureValue = analogRead(SoilSensor);
  soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);

  tb.sendTelemetryInt("lux", lightMeter.readLightLevel());
  tb.sendTelemetryFloat("temperature", dht.readTemperature());
  tb.sendTelemetryFloat("humidity", dht.readHumidity());
  tb.sendTelemetryFloat("soil moisture", soilmoisturepercent);

  tb.loop();
 
  delay(10000);
  
 //ESP32 Deep Sleep
  Serial.println("ESP is now sleeping for 1 hour");
  ESP.deepSleep(3600e6);
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
