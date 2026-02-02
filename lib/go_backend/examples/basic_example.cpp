/*
 * Example: ESP8266 with Go Backend Integration
 * 
 * This example demonstrates how to use the go_backend library
 * to send sensor data to the SKVMS Go backend server
 */

#include "config.h"

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif

#include "battery_monitor.h"
#include "wifi_configs.h"
#include "go_backend.h"

// WiFi Configuration
WiFiConfigs Wifi_configs;

// Battery Monitor
BatteryMonitor Battery_monitor(battery_voltage_pin);

// Go Backend Client
#if defined(USE_GO_BACKEND)
GoBackend backend(BACKEND_HOST, BACKEND_PORT);
#endif

// Data struct
Data sensor_data;

void setup() {
  Serial.begin(9600);
  Serial.println("\n\n========== ESP8266 Go Backend Example ==========");
  
  // Initialize sensor data
  sensor_data.led_relayState = "off";
  
  // Connect to WiFi
  Serial.println("Connecting to WiFi...");
  Wifi_configs.connect();
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
  
#if defined(USE_GO_BACKEND)
  // Initialize backend connection
  if (backend.begin()) {
    Serial.println("Backend initialized!");
    
    // Register this device with the backend server
    String macAddress = WiFi.macAddress();
    if (backend.registerDevice(DEVICE_NAME, DEVICE_TYPE, macAddress.c_str(), 1)) {
      Serial.print("Device registered with ID: ");
      Serial.println(backend.getDeviceID());
    } else {
      Serial.println("Failed to register device!");
    }
    
    // Set custom send interval if needed
    backend.setSendInterval(SEND_INTERVAL);
  } else {
    Serial.println("Failed to initialize backend!");
  }
#endif
}

void loop() {
  // Read sensor data
  sensor_data.battery_voltage = Battery_monitor.get_voltage();
  
  // Print current voltage
  Serial.print("Battery Voltage: ");
  Serial.print(sensor_data.battery_voltage);
  Serial.println(" V");
  
#if defined(USE_GO_BACKEND)
  // Check if it's time to send data
  if (backend.shouldSendData()) {
    Serial.println("\n--- Sending data to backend ---");
    
    // Option 1: Send voltage directly
    if (backend.sendReading(sensor_data.battery_voltage, 0.0)) {
      Serial.println("Data sent successfully!");
    } else {
      Serial.println("Failed to send data!");
    }
    
    // Option 2: Send complete Data struct
    // backend.sendData(sensor_data);
  }
#endif
  
  delay(5000);  // Read sensors every 5 seconds
}
