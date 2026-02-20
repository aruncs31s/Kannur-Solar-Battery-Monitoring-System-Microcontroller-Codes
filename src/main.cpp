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
#include "handlers.h"

#if defined(USE_GO_BACKEND)
#include "go_backend.h"
#endif

// For Setup Wizard
#include <LiquidCrystal_I2C.h>

// Data struct
Data new_data;

bool led_relayState = false;

// Solar Monitor Server Object
SolarMonitorServer Solar_monitor_server;

WiFiConfigs Wifi_configs;

BatteryMonitor Battery_monitor(battery_voltage_pin);

// Handler object that creates and manages the server
// NOTE: Initialized in setup() after WiFi is connected, not as global
Handlers* handlers = nullptr;

#if defined(USE_GO_BACKEND)
// Go Backend Client
GoBackend backend(
    BACKEND_HOST,
    BACKEND_PORT,
    TOKEN);
bool backend_initialized = true;

#endif

extern LiquidCrystal_I2C lcd;
extern bool lcd_initialized;
extern void setup_lcd();

void setup()
{
  init_serial();
  setup_lcd();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");
  // Initialize relay
  led_relayState = false;
  Solar_monitor_server.init_relay();

#if defined(STATIC_IP)
  Wifi_configs.get_static_ip();
#endif

  Wifi_configs.connect();

  // Wait for WiFi connection
  int retry_count = 0;
  while (WiFi.status() != WL_CONNECTED && retry_count < RETRY_COUNT)
  {
    delay(500);
    Serial.print(".");
    retry_count++;
  }
#if defined(DEBUG)
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nWiFi connected successfully!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("MAC Address: ");
    Serial.println(WiFi.macAddress());
  }
#endif

  // Initialize the HTTP server AFTER WiFi is connected
  if (WiFi.status() == WL_CONNECTED)
  {
    handlers = new Handlers();
  }
}

void update_reading()
{
  new_data.battery_voltage = Battery_monitor.get_voltage();
}

void loop()
{
  update_reading();

  // Handle local web server requests
  if (handlers != nullptr && WiFi.status() == WL_CONNECTED)
  {
    handlers->handleClient();
  }

#if defined(USE_GO_BACKEND)
  // Send data to backend at configured intervals
  if (backend_initialized)
  {
#if defined(DEBUG)
    Serial.println("\n--- Sending to Backend ---");
    Serial.print("Voltage: ");
    Serial.print(new_data.battery_voltage);
    Serial.println(" V");
#endif
    if (lcd_initialized)
    {
      lcd.setCursor(0, 1);
      lcd.print("Sent to Backend!   ");
    }

    if (backend.sendReading(new_data.battery_voltage, 0.0))
    {
      if (lcd_initialized)
      {
        lcd.setCursor(0, 1);
        lcd.print("Data Sent!         ");
      }
#if defined(DEBUG)
      Serial.println("✓ Data sent successfully!");
#endif
    }
    else
    {
      if (lcd_initialized)
      {
        lcd.setCursor(0, 1);
        lcd.print("Failed to send!    ");
      }
#if defined(DEBUG)
      Serial.println("✗ Failed to send data!");
#endif
    }

    Serial.println();
  }
  else
  {
    if (lcd_initialized)
    {
      lcd.setCursor(0, 1);
      lcd.print("Backend Init Failed");
    }
    #if defined(DEBUG)
    Serial.println("Initializing Backend  ...");
    #endif
  }
#endif
  delay(SEND_INTERVAL);
}
