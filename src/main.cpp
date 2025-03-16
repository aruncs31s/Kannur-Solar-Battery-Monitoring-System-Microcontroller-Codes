/*
 * Time : 20:25
 * Date : 2025-03-15
 */

#include "config.h"
#include <Arduino.h>

#if defined(ESP8266)
// Wifi.h will not work for ESP8266
#include <ESP8266WiFi.h>
#elif defined(ESP32)
// For ESP32
#include <WiFi.h>
#endif

#include "battery_monitor.h"
#include "solar_monitor_server.h"
#include "wifi_configs.h"

// All the function Definition Will Go here

// Do something about this

WiFiServer server(8090);

String header;
// For wifi Server

unsigned long currentTime = millis();
unsigned long previousTime = 0;

// Data struct
Data new_data;

String led_relayState = "off";
// Solar Monitor Server Object

SolarMonitorServer Solar_monitor_server;

WiFiConfigs Wifi_configs;

BatteryMonitor Battery_monitor(battery_voltage_pin);

// Reconnect if lost connection

void setup() {
  Serial.begin(9600);
  // Get static IP
  new_data.led_relayState = "off";
  Solar_monitor_server.init_relay();
#if defined(STATIC_IP)
  Wifi_configs.get_static_ip();
#endif
  Wifi_configs.connect();
}

void update_reading() {
  new_data.battery_voltage = Battery_monitor.get_voltage();
}

void loop() {
  update_reading();
  WiFiClient client = server.available();

  if (client) {
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");
    String currentLine = "";
    while (client.connected() && currentTime - previousTime <= TIMEOUT_MS) {
      currentTime = millis();
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        header += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {
            if (header.indexOf("GET /data") >= 0 ||
                header.indexOf("GET /data/?=") >= 0) {
#if defined(DEBUG_EVERYTHING)
              Serial.println("Serving Json Response");
#endif
              Solar_monitor_server.update_json_response(client, new_data);

              if (header.indexOf("GET /data?=on") >= 0) {
                Serial.println("GPIO D4 on");
                // header = "GET /data";
                // digitalWrite(LED_BUILTIN, HIGH);
                Solar_monitor_server.turn_on_off_relay(0);
              } else if (header.indexOf("GET /data?=off") >= 0) {
                Serial.println("GPIO D4 off");
                // header = "GET /data";
                // digitalWrite(LED_BUILTIN, LOW);
                Solar_monitor_server.turn_on_off_relay(1);
              }
              break;
            } else {
#if defined(DEBUG_EVERYTHING)
              Serial.println("Serving HTML");
#endif
              Solar_monitor_server.present_website(client, new_data);
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
  delay(100);
}
