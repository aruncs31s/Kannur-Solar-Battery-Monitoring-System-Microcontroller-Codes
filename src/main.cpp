/*
* V0.0.3 
* Start Date: 06-01-2025
*/

#include <Arduino.h>
#include "config.h"
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif
#include <wifi_configs.h>
#include "solar_monitor_server.h"



Data theData;
WiFiConfigs theWifi;

SolarMonitorServer theServer;

#define CLIENT_TIMEOUT 1000
WiFiServer server(80);
String header;
unsigned long currentTime = 0;
unsigned long previousTime = 0;


void setup(){
    Serial.begin(BAUD_RATE);
    // Random Values for the testing
    theData.battery_voltage = 100;
    theData.led_relayState = 1;
    // Connect to wifi
    theWifi.connect();
    // Begin the server
    theServer.init_relay();
    server.begin();

}

void loop(){
  Serial.println();
#if defined(DEBUG)
    Serial.println("Inside the loop()");
#endif
    WiFiClient client = server.available();
    theServer.sendDataToServer(theData);
    delay(P_DELAY);

    if (client) {
    currentTime = millis();
    previousTime = currentTime;

    Serial.println("New Client.");

    String currentLine = "";
    while (client.connected() && currentTime - previousTime <= CLIENT_TIMEOUT) {
      currentTime = millis();
      if (client.available()) {
        char c = client.read();
        // Serial.write(c);
        header = header + c;
        if (c == '\n') {
          if (currentLine.length() == 0) {
            if (header.indexOf("GET /data") >= 0 ||
                header.indexOf("GET /data/?=") >= 0) {
                theServer.update_json_response(client, theData);
              if (header.indexOf("GET /data?=on") >= 0) {
                Serial.println("GPIO D4 on");
                theServer.turn_on_off_relay(0);
              } else if (header.indexOf("GET /data?=off") >= 0) {
                Serial.println("GPIO D4 off");
                // header = "GET /data";
                // digitalWrite(LED_BUILTIN, LOW);
                theServer.turn_on_off_relay(1);
              }
              break;
            } else {
              theServer.present_website(client, theData);
              break;
            }
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    header = "";
    client.stop();
#if defined(DEBUG_EVERYTHING)
    Serial.println("Client disconnected.");
#endif
  }

}