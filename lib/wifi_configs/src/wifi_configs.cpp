#include "wifi_configs.h"
#include "config.h"


#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#else
#error "No WiFi header file found"
#endif
void WiFiConfigs::connect() {
  const char *ssid = WIFI_SSID;
  const char *password = WIFI_PASSWORD;
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
#if defined(DEBUG_EVERYTHING)
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
#endif
}

void WiFiConfigs::reconnect() {
  while ((WiFi.status()) != WL_CONNECTED) {
    Serial.println("Reconnecting .");
    WiFi.disconnect();
    WiFi.reconnect();
    delay(5000);
  }
}

#if defined(STATIC_IP)
void WiFiConfigs::get_static_ip() {
  IPAddress local_IP(STATIC_IP_ADDRESS);
  IPAddress gateway(STATIC_IP_GATEWAY);
  IPAddress subnet(STATIC_IP_SUBNET);
  IPAddress primaryDNS(8, 8, 8, 8);
  IPAddress secondaryDNS(8, 8, 8, 8);
  WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
}
#endif
