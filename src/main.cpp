/*
* V0.0.3 
* Start Date: 06-01-2025
*/

#include <Arduino.h>
#include "config.h"
#include <ESP8266WiFi.h>
#include <wifi_configs.h>
#include "solar_monitor_server.h"

Data theData;
WiFiConfigs theWifi;
SolarMonitorServer theServer;
void setup(){
    Serial.begin(BAUD_RATE);
    theData.battery_voltage = 100;
    theData.led_relayState = 1;
    theWifi.connect();
}
void loop(){
    Serial.println("Hi");
    delay(1000);
    theServer.sendDataToServer(theData);
}