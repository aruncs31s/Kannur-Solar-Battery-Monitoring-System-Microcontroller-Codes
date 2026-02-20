#include "data.h"


Data ReadingsRepo::getAllReadings() {
    return _current_readings;
}
void ReadingsRepo::updateVoltage(float voltage) {
    _current_readings.battery_voltage = voltage;
}
void ReadingsRepo::updateLedState(LedState state) {
    _current_readings.led_relayState = (state == LedState::ON);
}
void ReadingsRepo::updateChargingCurrent(float current) {
    _current_readings.charging_current = current;
}
