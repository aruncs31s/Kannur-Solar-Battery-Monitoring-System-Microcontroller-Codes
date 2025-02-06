#ifndef __SOLAR_MONITOR_H__
#define __SOLAR_MONITOR_H__
#include <stdbool.h>

#if defined(ESP32)
#include <WiFi.h>
#else // ESP8266
#include <ESP8266WiFi.h>
#endif
#include "config.h"
class SolarMonitorServer {
public:
  void init_relay();
  void present_website(WiFiClient &client, Data &new_data);
  void update_json_response(WiFiClient &client, Data &new_data);
  void turn_on_off_relay(bool _state);
  // This is the never method and it should not be used alone , this is actualy in testing mode and and the library will be updated , unwanted codes wil be removed
  void sendDataToServer(Data &data);
private:
  const uint8_t _led_relayPin = led_relayPin;
};

#endif // !__SOLAR_MONITOR_H__
