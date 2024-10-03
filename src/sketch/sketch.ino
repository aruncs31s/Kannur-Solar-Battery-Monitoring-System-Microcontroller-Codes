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

// Humid Temp Sensor
#if defined(HUMID_TEMP_SENSING)
#include "humid_temp_sensor.h"
// Humid Temp Sensor Object
humidTempSensor Humid_Temp_Sensor;
#endif

// Light Sensor
#if defined(LIGHT_SENSING)
#include "light_sensor.h"
// Light sensor Object
lightSensor Light_Sensor;
#endif

// LCD Display
#if defined(LCD_DSPLAY)
#include "LiquidCrystal_I2C.h"
// LCD Display object
LiquidCrystal_I2C lcd(0x27, 16, 2);
#endif

#if defined(RAIN_SENSING)
#include "weather_station.h"

weatherStation Weather_Station;
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
#if defined(Light_SENSING)

  // Initialize Light Sensor
  Light_Sensor.begin();

#endif
#if defined(HUMID_TEMP_SENSING)
  // Initialze Humid Temp Sensor
  Humid_Temp_Sensor.begin();
#endif
  // Start Server
#if defined(RAIN_SENSING) || defined(WIND_DIRECTION_SENSING) ||                \
    defined(WIND_SPEED_SENSING)
  Weather_Station.init();
#endif
  server.begin();

#if defined(LCD_DSPLAY)
  // Iniitlize the lcd
  lcd.init();
  lcd.clear();
  lcd.backlight();
#endif
}

void update_reading() {

  new_data.battery_voltage = Battery_monitor.get_voltage();
#if defined(HUMID_TEMP_SENSING)
  Humid_Temp_Sensor.get_readings();
  new_data.humidity = Humid_Temp_Sensor.humidity;
  new_data.temperature = Humid_Temp_Sensor.temperature;
#endif
#if defined(LIGHT_SENSING)
  new_data.light_sensor_value = Light_Sensor.get_value();
#endif
#if defined(WIND_SPEED_SENSING)
  new_data.wind_speed = Weather_Station.get_speed();
#endif
#if defined(WIND_DIRECTION_SENSING)
 new_data.wind_direction = Weather_Station.get_direction();
#endif
}

void loop() {
  update_reading();
  new_data.rain_volume = Weather_Station.get_rain();
  Serial.println("Rain Volument =" + String(new_data.rain_volume));
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
                Solar_monitor_server.turn_on_off_relay(1);
              } else if (header.indexOf("GET /data?=off") >= 0) {
                Serial.println("GPIO D4 off");
                // header = "GET /data";
                // digitalWrite(LED_BUILTIN, LOW);
                Solar_monitor_server.turn_on_off_relay(0);
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
}
