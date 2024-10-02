#ifndef __SOLAR_MONITOR_H__
#define __SOLAR_MONITOR_H__
#if defined(ESP32)
#include <WiFi.h>
#else // ESP8266
#include <ESP8266WiFi.h>
#endif
#include "config.h"
class SolarMonitorServer {
public:
  void present_website(WiFiClient &client, Data &new_data);
  void update_json_response(WiFiClient &client, Data &new_data);

private:
};
#endif // !__SOLAR_MONITOR_H__
