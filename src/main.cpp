/*
 * SKVMS ESP8266 -
 * Time : 13:31
 * Date : 2024-10-02
 * Updated: 2026-02-02 - Added Go Backend Integration
 */

#include "config.h"
#include "initializer.h"
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

#if defined(USE_GO_BACKEND)
#include "go_backend.h"
#endif


// For Setup Wizard
#include <LiquidCrystal_I2C.h>

// WiFi Server for local web interface
WiFiServer server(80);
String header;

unsigned long currentTime = millis();
unsigned long previousTime = 0;

// Data struct
Data new_data;

String led_relayState = "off";

// Solar Monitor Server Object
SolarMonitorServer Solar_monitor_server;

WiFiConfigs Wifi_configs;

BatteryMonitor Battery_monitor(battery_voltage_pin);

#if defined(USE_GO_BACKEND)
// Go Backend Client
GoBackend backend(BACKEND_HOST, BACKEND_PORT,TOKEN);
bool backend_initialized = true;
#endif

extern LiquidCrystal_I2C lcd;
extern void setup_lcd();

void setup() {
  init_serial();
  setup_lcd();

  // Initialize relay
  new_data.led_relayState = "off";
  Solar_monitor_server.init_relay();

#if defined(STATIC_IP)
  Wifi_configs.get_static_ip();
#endif

 
  Wifi_configs.connect();

  // Wait for WiFi connection
  int retry_count = 0;
  while (WiFi.status() != WL_CONNECTED && retry_count < RETRY_COUNT) {
    delay(500);
    Serial.print(".");
    retry_count++;
  }


  if (WiFi.status() == WL_CONNECTED) {

#if defined(DEBUG)
     Serial.println("\nWiFi connected successfully!");
     Serial.print("IP Address: ");
     Serial.println(WiFi.localIP());
      Serial.print("MAC Address: ");
      Serial.println(WiFi.macAddress());
#endif
    // Start local web server
    server.begin();
#if defined(DEBUG)
    Serial.println("Local web server started");
#endif
  } else {
#if defined(DEBUG)
    Serial.println("\nWiFi connection failed!");
#endif
  }

}

void update_reading() {
  new_data.battery_voltage = Battery_monitor.get_voltage();
}

void handleClient() {
  WiFiClient client = server.accept();

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
            // HTTP response
            if (header.indexOf("GET /data") >= 0) {
              // Return JSON data
              Solar_monitor_server.update_json_response(client, new_data);
            } else if (header.indexOf("GET /on") >= 0) {
              new_data.led_relayState = "on";
              Solar_monitor_server.turn_on_off_relay(true);
              Solar_monitor_server.present_website(client, new_data);
            } else if (header.indexOf("GET /off") >= 0) {
              new_data.led_relayState = "off";
              Solar_monitor_server.turn_on_off_relay(false);
              Solar_monitor_server.present_website(client, new_data);
            } else {
              // Default homepage
              Solar_monitor_server.present_website(client, new_data);
            }
            break;
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
    Serial.println("Client disconnected.\n");
  }
}

void loop() {
  update_reading();

#if defined(USE_GO_BACKEND)
  // Send data to backend at configured intervals
  if (backend_initialized ) { //&& backend.shouldSendData()
    Serial.println("\n--- Sending to Backend ---");
    Serial.print("Voltage: ");
    Serial.print(new_data.battery_voltage);
    Serial.println(" V");

    if (backend.sendReading(new_data.battery_voltage, 0.0)) {
      Serial.println("✓ Data sent successfully!");
    } else {
      Serial.println("✗ Failed to send data!");
    }
    Serial.println();
  } else {
    Serial.println("Initializing Backend  ...");
  }
#endif
  delay(SEND_INTERVAL);

  // Handle local web server requests
  // handleClient();
}
