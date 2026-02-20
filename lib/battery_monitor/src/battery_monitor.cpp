#include "battery_monitor.h"

float BatteryMonitor::get_voltage() {

  if (!_is_time_for_reading()) {
    return _prev_voltage;
  }
  // Read the voltage from the pin
  uint16_t raw = analogRead(_voltage_adc_pin);

  // Serial.println("Raw value: " + String(raw));
  raw = raw + ADC_OFFSET;
  // Convert the raw value to a voltage
  float voltage = (raw * (R1 + R2) * V_REF) / ADC_MAX;
  _prev_voltage = voltage;
  if (voltage > _max_voltage) {
    _max_voltage = voltage;
  }
  if (voltage < _min_voltage) {
    _min_voltage = voltage;
  }
  return voltage;
}

float BatteryMonitor::get_max_voltage() {
  return _max_voltage;
}

float BatteryMonitor::get_min_voltage(){
  return _min_voltage;
}

float BatteryMonitor::get_charging_current() {
  // Placeholder implementation, replace with actual current measurement logic
  return _charging_current;
}

bool BatteryMonitor::_is_time_for_reading(){
   {
    unsigned long current_time = millis();
    return (current_time - _prev_reading_time) >= _time_between_readings;
  }
}