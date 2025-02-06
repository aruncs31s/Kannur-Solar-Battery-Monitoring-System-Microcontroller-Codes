/*
* V0.0.3 
* Start Date: 06-01-2025
*/

#include <Arduino.h>
#include "config.h"
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

void sendDataToServer(Data &data) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client;
    
    // Replace with your server's IP and port
    http.begin(client, "http://your_server_ip:5000/api/data");
    http.addHeader("Content-Type", "application/json");
    
    // Create JSON document
    StaticJsonDocument<200> doc;
    // doc["temperature"] = data.temperature;
    // doc["humidity"] = data.humidity;
    // doc["light_sensor_value"] = data.light_sensor_value;
    doc["battery_voltage"] = data.battery_voltage;
    // doc["led_relayState"] = data.led_relayState;
    // doc["rain_volume"] = data.rain_volume;
    
    String jsonString;
    serializeJson(doc, jsonString);
    
    // Send POST request
    int httpResponseCode = http.POST(jsonString);
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("HTTP Response code: " + String(httpResponseCode));
      Serial.println(response);
    } else {
      Serial.println("Error on sending POST: " + String(httpResponseCode));
    }
    
    http.end();
  }
}
void setup(){
    Serial.begin(BAUD_RATE);
}
void loop(){
    Serial.println("Hi");
    delay(1000);
}