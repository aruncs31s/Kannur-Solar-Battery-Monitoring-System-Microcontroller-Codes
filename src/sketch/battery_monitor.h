#ifndef __BATTERY_MONITOR_H__
#define __BATTERY_MONITOR_H__

#include <cstdint>
#include <cstdio>
class BatteryMonitor {
public:
  BatteryMonitor(uint8_t pin) : _pin(pin) {}
  float get_voltage();

private:
  uint8_t _pin;
};

#endif // __BATTERY_MONITOR_H__
