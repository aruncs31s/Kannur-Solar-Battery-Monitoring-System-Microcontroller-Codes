#if !defined(DATA_H)
#define DATA_H
#include <Arduino.h>

typedef struct Data {
#if defined(LIGHT_SENSING)
  float light_sensor_value;
#endif
#if defined(HUMID_TEMP_SENSING)
  float humidity;
  float temperature;
#endif
  float battery_voltage;
#if defined(RAIN_SENSING)
  float rain_volume;
#endif
#if defined(WIND_SPEED_SENSING)
  float wind_speed;
#endif
#if defined(WIND_DIRECTION_SENSING)
  float wind_direction;
#endif
  bool led_relayState;
  float charging_current;
} Data;


enum class DataType {
    BATTERY_VOLTAGE,
    LED_RELAY_STATE,
    CHARGING_CURRENT
};
enum class LedState {
    OFF,
    ON,
};

class ReadingsRepo {
    public: 
    ReadingsRepo() {
        _current_readings.battery_voltage = 0.0;
        _current_readings.led_relayState = false;
        _current_readings.charging_current = 0.0;
    }
        Data getAllReadings();
        void updateVoltage(float voltage);
        void updateLedState(LedState state);
        void updateChargingCurrent(float current);
    private:
        Data _current_readings;
};

#endif // __DATA_H_