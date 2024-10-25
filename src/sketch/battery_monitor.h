#ifndef __BATTERY_MONITOR_H__
#define __BATTERY_MONITOR_H__

#include <cstdint>
#include <cstdio>
class BatteryMonitor {
public:
  BatteryMonitor(uint8_t pin) : _pin(pin) {}
  float get_voltage();
  void get_adc_voltage();

private:
  uint8_t _pin;
  float _adc_voltage = 0;
  uint16_t raw;
};
#endif // __BATTERY_MONITOR_H__
