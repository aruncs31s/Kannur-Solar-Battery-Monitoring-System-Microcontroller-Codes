#ifndef __WIFI_CONFIGS_H_
#define __WIFI_CONFIGS_H_
#include <stdlib.h>

enum {
    WIFI_CONNECTED = 1,
    WIFI_DISCONNECTED = 0,
  };

class WiFiConfigs {
public:
  void connect();
  void reconnect();
  void get_static_ip();

private:
  // Store Status
  int8_t wifi_status;
};
#endif // __WIFI_CONFIGS_H_
