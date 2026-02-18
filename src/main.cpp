/*
 * SKVMS ESP8266 -
 * Time : 13:31
 * Date : 2024-10-02
 * Updated: 2026-02-02 - Added Go Backend Integration
 */

#include "config.h"

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

#if defined(USE_OTA)
#include <ArduinoOTA.h>
#endif

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

void setup() {
  Serial.begin(115200);
  Serial.println("\n\n========== SKVMS ESP8266 Starting ==========");

  // Initialize relay
  new_data.led_relayState = "off";
  Solar_monitor_server.init_relay();

#if defined(STATIC_IP)
  Wifi_configs.get_static_ip();
#endif

  // Connect to WiFi
  Serial.println("Connecting to WiFi...");
  Wifi_configs.connect();

  // Wait for WiFi connection
  int retry_count = 0;
  while (WiFi.status() != WL_CONNECTED && retry_count < 20) {
    delay(500);
    Serial.print(".");
    retry_count++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("MAC Address: ");
    Serial.println(WiFi.macAddress());

    // Start local web server
    server.begin();
    Serial.println("Local web server started");
  } else {
    Serial.println("\nWiFi connection failed!");
  }

#if defined(USE_OTA)
  // Initialize OTA
  Serial.println("\n--- Initializing OTA ---");
  ArduinoOTA.setPort(OTA_PORT);
  ArduinoOTA.setHostname(OTA_HOSTNAME);
  
#if defined(OTA_PASSWORD)
  ArduinoOTA.setPassword(OTA_PASSWORD);
  Serial.println("OTA Password: Enabled");
#endif

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }
    Serial.println("\n[OTA] Start updating " + type);
  });
  
  ArduinoOTA.onEnd([]() {
    Serial.println("\n[OTA] Update complete!");
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("[OTA] Progress: %u%%\r", (progress / (total / 100)));
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("[OTA] Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  
  ArduinoOTA.begin();
  Serial.println("OTA Ready!");
  Serial.print("Hostname: ");
  Serial.println(OTA_HOSTNAME);
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
#endif
  
  Serial.println("========== Setup Complete ==========\n");
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
#if defined(USE_OTA)
  // Handle OTA updates
  ArduinoOTA.handle();
#endif

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
