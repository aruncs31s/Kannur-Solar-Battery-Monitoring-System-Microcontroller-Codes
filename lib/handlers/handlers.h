#include "Arduino.h"

// The Server

#include "solar_monitor_server.h"

extern SolarMonitorServer Solar_monitor_server;

// Data struct
extern Data new_data;

#if defined(ESP8266)
// Wifi.h will not work for ESP8266
#include <ESP8266WiFi.h>
#elif defined(ESP32)
// For ESP32
#include <WiFi.h>
#endif



class Handlers {
public:
  // Handler for the root endpoint
  static void handleRoot();
  void handleClient();
  
  // Constructor that creates and starts the server on default port 80
  Handlers();
  
  // Constructor that creates and starts the server on specified port
  Handlers(uint16_t port);

private:
  unsigned long _currentTime = millis();
  unsigned long _previousTime = 0;
  String _header;
  WiFiServer _server;
};
