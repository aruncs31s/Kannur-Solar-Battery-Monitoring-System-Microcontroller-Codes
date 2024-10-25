#include "battery_monitor.h"
#include "config.h"
float BatteryMonitor::get_voltage() {
  // Read the voltage from the pin
  raw = analogRead(_pin);

  Serial.println("Raw value: " + String(raw));
  raw = raw + ADC_OFFSET;
  return (_adc_voltage - 0.17) * (R1 + R2);
  // Convert the raw value to a voltage
  // return ((raw * (R1 + R2) * V_REF) / ADC_MAX);
}
void BatteryMonitor::get_adc_voltage() {
  _adc_voltage = (raw * 3.3) / ADC_MAX;
  // return _adc_voltage;
}
