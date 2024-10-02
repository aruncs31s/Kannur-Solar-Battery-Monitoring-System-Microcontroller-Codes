#ifndef __CONFIG_H_
#define __CONFIG_H_
#include <Arduino.h>
typedef struct Data {
  float light_sensor_value;
  float humidity;
  float temperature;
  float battery_voltage;
  String led_relayState;
} Data;

#endif //!__CONFIG_H_
