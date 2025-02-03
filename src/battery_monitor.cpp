#include "battery_monitor.h"
#include "config.h"
float BatteryMonitor::get_voltage() {
  // Read the voltage from the pin
  uint16_t raw = analogRead(_pin);

  Serial.println("Raw value: " + String(raw));
  raw = raw + ADC_OFFSET;
  // Convert the raw value to a voltage
  return (raw * (R1 + R2) * V_REF) / ADC_MAX;
}
