/*
 * Time : 13:31 
 * Date : 2024-10-02
 */

#include "LiquidCrystal_I2C.h"
#include "config.h"
#include "humid_temp_sensor.h"
#include "light_sensor.h"
#include "solar_monitor_server.h"
#include <ESP8266WiFi.h>

// All the function Definition Will Go here

void check_wifi();
void present_webPage(WiFiClient *client);
void send_json_response(WiFiClient *client);



String led_relayState = "off";

const int relay_pin = D4;
const char *ssid = "802.11";
const char *password = "12345678p";
WiFiServer server(80);

String header;
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;


// Light sensor Object
lightSensor Light_Sensor;

// Data struct
Data new_data;
// LCD Display object 
LiquidCrystal_I2C lcd(0x27, 16,2); 

// Humid Temp Sensor Object
humidTempSensor Humid_Temp_Sensor;


// Solar Monitor Server Object
SolarMonitorServer Solar_monitor_server;


// Reconnect if lost connection
void reconnect() {
  if ((WiFi.status()) != WL_CONNECTED) {
    Serial.println("Reconnecting .");
    WiFi.disconnect();
    WiFi.reconnect();
    delay(5000);
  }
}

void setup() {
  Serial.begin(9600);
  // Get static IP
  pinMode(relay_pin,OUTPUT);
  IPAddress local_IP(192, 168, 67, 50);
  IPAddress gateway(192, 168, 67, 1);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress primaryDNS(8, 8, 8, 8);
  IPAddress secondaryDNS(8, 8, 8, 8);
  WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
  check_wifi();
  Light_Sensor.begin();
  Humid_Temp_Sensor.begin();
  // Start Server
  server.begin();
  // Iniitlize the lcd
  lcd.init();
  lcd.clear();
  lcd.backlight();
}
void debug() {
  Serial.println("Humid " + String(new_data.humidity));
  Serial.println("Temp " + String(new_data.temperature));
  Serial.println("Light " + String(new_data.light_sensor_value));
}

void update_reading(){
  Humid_Temp_Sensor.get_readings();
  new_data.light_sensor_value = Light_Sensor.get_value();
  new_data.temperature = Humid_Temp_Sensor.temperature;
  new_data.humidity = Humid_Temp_Sensor.humidity;
  new_data.battery_voltage = (((analogRead(A0)) * 11.0 * 3.3) / 1024.0);
}
void loop() {
  update_reading();

  lcd.setCursor(2, 0);
  lcd.print(WiFi.localIP());
  WiFiClient client = server.available();

  lcd.setCursor(2, 1); // Set cursor to character 2 on line 0
  lcd.print("Volt = ");
  lcd.print(new_data.battery_voltage);
  lcd.print(" V");

  header = "";
  if (client) {
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");
    String currentLine = "";
    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        header += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {
            if (header.indexOf("GET /data") >= 0 || header.indexOf("GET /data/?=") >= 0 || header.indexOf("GET /data?=off") >= 0) {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: application/json");
              client.println("Connection: close");
              client.println();
              Solar_monitor_server.update_json_response(client,new_data);
              client.println();
              if (header.indexOf("GET /data?=on") >= 0) {
              Serial.println("GPIO D4 on");
              // header = "GET /data";
                           
              led_relayState = "on";
              digitalWrite(LED_BUILTIN, HIGH);
              } else if(header.indexOf("GET /data?=off") >= 0) {
              Serial.println("GPIO D4 off");
              // header = "GET /data";
              led_relayState = "off";
              digitalWrite(LED_BUILTIN, LOW);
              } 
              break;
              } else {
              Solar_monitor_server.present_website(client,new_data);
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
    Serial.println("Client disconnected.");
    Serial.println("");
      }
}

void check_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


