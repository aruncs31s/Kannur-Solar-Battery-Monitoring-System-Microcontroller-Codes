#ifndef __BATTERY_MONITOR_H__
#define __BATTERY_MONITOR_H__
#include <Arduino.h>
#include "config.h"
class BatteryMonitor {
public:
  BatteryMonitor(uint8_t voltageADCPin) : _voltage_adc_pin(voltageADCPin) {}
  float get_voltage();
  float get_max_voltage();
  float get_min_voltage();
  float get_charging_current();


private:
  bool _is_time_for_reading();

  uint8_t _voltage_adc_pin;
  float _prev_voltage = 0.0;
  float _max_voltage = 0.0;
  float _min_voltage = 0.0;
  float _charging_current = 0.0;
  unsigned long  _prev_reading_time = 0.0;
  // TODO: Make this configurable
  unsigned long _time_between_readings = 1000; // 5 second
};
#endif // __BATTERY_MONITOR_H__
