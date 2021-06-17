/*
GIPE 2021 Smart Farming IoT Code
==================================
Last Updated 17 June 2021

Parts :
- ESP 32
- DHT 11
- LDR
- Soil Moisture Sensor v1.2 (https://makersportal.com/blog/2020/5/26/capacitive-soil-moisture-calibration-with-arduino)

Lib:
- dht11 lib by adafruit
- https://thingsboard.io/docs/samples/esp32/gpio-control-pico-kit-dht22-sensor/

IMPORTANT IF YOU WANNA TEST USING ESP8266::
- DO NOT UPDATE YOUR ESP8266 BOARD DRIVER(bellow board manager) TO 3.0.0!!!!! USE 2.7.4 INSTEAD OR YOU WILL GET AN ERROR FROM THINGSBOARD!!!
- YOU WILL GET AN ERROR WITH ANOTHER ANALOG INPUT SINCE ESP8266 HAS ONLY 1 ANALOG INPUT AND WE USE 2 ANALOG INPUT; THEREFORE ANOTHER ANALOG DOES NOT EXIST

IMPORTANT IF YOU USE ESP 32:
- IF YOURE DUMMY LIKE ME AND HAVE PROBLEM UPLOADING CODE HERE IS THE SOLUTION https://randomnerdtutorials.com/getting-started-with-esp32/

==================================
Made by:
GIPE 2021 Scholars

Tested on DOIT ESP 32 DEV Kit V1 board 25 May 2021
*/

#include "ThingsBoard.h"
#include <WiFi.h>           //Setup for ESP32 change to <ESP8266WiFi.h> if you want to use ESP8266(WARNING ESP 8266 has only 1 Analog input!)
//setup ISP
#define WIFI_AP             "xxxxx"            //PUSPITA AP
#define WIFI_PASSWORD       "*********"        //Password
//setup Token
#define DESIRED_TOKEN       "node3"           // change this according to desired node (TOKEN)
#define TOKEN               DESIRED_TOKEN           
#define THINGSBOARD_SERVER  "smart-farming.puspanita.id" //our thingsboard server
// Baud rate for debug serial
#define SERIAL_DEBUG_BAUD   115200

// Initialize ThingsBoard client
WiFiClient espClient;
// Initialize ThingsBoard instance
ThingsBoard tb(espClient);
// the Wifi radio's status
int status = WL_IDLE_STATUS;

//Setup Sensors and Pins

//DHT11
#include "DHT.h"
#define DHTPIN 26            //DHT Pin
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float h = 0, t = 0;

//LDR
#define LDR 34              //LDR Pin (Analog PIN)
int light =0;
float Lux =0, adc = 0;

//Soil Moisture
#define SoilMoisturePin 35  //Soil Moisture Pin (MUST BE ANALOG PIN)
const int AirValue = 3500;   //you need to replace this value through calibration, read it when the sensor is placed in open air
const int WaterValue = 1720; //you need to replace this value through calibration, read it when the sensor is drowned in water
int soilMoistureValue = 0;
int soilmoisturepercent = 0;

void setup() {
  // initialize serial for debugging
  Serial.begin(SERIAL_DEBUG_BAUD);
  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  InitWiFi();
  //DHT11
  dht.begin();
  //Soil Moisture
  pinMode(SoilMoisturePin, INPUT);
  //LDR
  pinMode(LDR, INPUT);
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

  Serial.print("\nSending data to: ");
  Serial.println(DESIRED_TOKEN);
  
  dht_readsend();
  delay(100);
  soilmoisture_readsend();
  delay(100);
  LDR_readsend();
  delay(100);
  test();  
  delay(500);
  
  tb.loop();
}


  void test()
  {
    Serial.print("\nHumidity =");
    Serial.println(h);
    Serial.print("Temperature =");
    Serial.println(t);

    Serial.print("Soilmoisture Value =");
    Serial.println(soilMoistureValue);
    Serial.print("Soilmoisture Percent =");
    Serial.println(soilmoisturepercent);

    Serial.print("Light =");
    Serial.println(light);
    Serial.print("ADC =");
    Serial.println(adc);
    Serial.print("Lux =");
    Serial.println(Lux);
  }
  

void dht_readsend(){
  h = dht.readHumidity();
  t = dht.readTemperature();

  tb.sendTelemetryFloat("temperature", t); 
  tb.sendTelemetryFloat("humidity", h);
}

void soilmoisture_readsend(){
  soilMoistureValue = analogRead(SoilMoisturePin);  //put Sensor insert into soil
  Serial.println(soilMoistureValue);
  soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);

  if(soilmoisturepercent >= 100)
  {
    Serial.println("100 %");
    tb.sendTelemetryInt("soil moisture", 100);
  }
  else if(soilmoisturepercent <=0)
  {
    Serial.println("0 %");
    tb.sendTelemetryInt("soil moisture", 0);
  }
  else if(soilmoisturepercent >0 && soilmoisturepercent < 100)
  {
    Serial.print(soilmoisturepercent);
    Serial.println("%"); 
    tb.sendTelemetryInt("soil moisture", soilmoisturepercent); 
  }
}

void LDR_readsend(){
  light = analogRead(LDR)+1;
  adc = 4096 - light;
  float ADC_value = 0.00080566406;

  Lux=(50.000000/(ADC_value*light));

  tb.sendTelemetryInt("lux", Lux); 
}

void InitWiFi()
{
  Serial.print("Connecting to AP ...");
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
