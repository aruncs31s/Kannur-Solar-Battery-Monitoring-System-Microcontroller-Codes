/*
 * Time : 13:31
 * Date : 2024-10-02
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

// LCD Display
#if defined(LCD_DSPLAY)
#include "LiquidCrystal_I2C.h"
// LCD Display object
LiquidCrystal_I2C lcd(0x27, 16, 2);
#endif

// All the function Definition Will Go here

// Do something about this

WiFiServer server(80);

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
  server.begin();

#if defined(LCD_DSPLAY)
  // Iniitlize the lcd
  lcd.init();
  lcd.clear();
  lcd.backlight();
#endif
}

void update_reading() {

  Battery_monitor.get_adc_voltage();
  new_data.battery_voltage = Battery_monitor.get_voltage();
}

void loop() {
  update_reading();
#if defined(LCD_DSPLAY)
  lcd.setCursor(2, 0);
  lcd.print(WiFi.localIP());
#endif
  WiFiClient client = server.available();

#if defined(LCD_DSPLAY)
  if (new_data.battery_voltage != Battery_monitor.get_voltage()) {
    lcd.setCursor(2, 1); // Set cursor to character 2 on line 0
    lcd.print("Volt = ");
    lcd.print(new_data.battery_voltage);
    lcd.print(" V");
  }
#endif

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
#if defined(DEBUG_EVERYTHING)
  Serial.print("Local IP ");
  Serial.println(WiFi.localIP());
#endif

// Serial.println("ADC Voltage = "+String(Battery_monitor.get_adc_voltage())); 
// delay(2000);
}
