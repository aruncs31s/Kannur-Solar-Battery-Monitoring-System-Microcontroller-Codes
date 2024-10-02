#ifndef __CONFIG_H_
#define __CONFIG_H_
#include "modules.h"
typedef struct Data {
#if defined(LIGHT_SENSING)
  float light_sensor_value;
#endif
#if defined(HUMID_TEMP_SENSING)
  float humidity;
  float temperature;
#endif
  float battery_voltage;
  bool led_relayStatus;

} Data;

#endif //!__CONFIG_H_
