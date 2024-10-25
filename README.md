# Kannur-Solar-Battery-Monitoring-System

Solar Battery Monitoring System for Kannur CCTV

## Features

1. Measures the voltage and displays it to a website hosted in the micro-controller itself
2. Graphical representation of the battery voltage
3. Manual `Street Light Control`

### philosophy

This is the `main` branch where the `most recent` stable release of the project will be contained

- `lates` branch contains the `latest` beta changes
- There will be a branch for `esp32` and a separate branch for `esp8266` and for `generic`

**\*generic** which is named because the code is written in a way that it will compile for both or all the boards using\*

```c
// config.h
#define ONEPLUS_AP
#define LIGHT_SENSING
#define HUMID_TEMP_SENSING
#define STATIC_IP
#define DEBUG_EVERYTHING
#define LCD_DSPLAY
#define RAIN_SENSING
#define WIND_DIRECTION_SENSING
#define WIND_SPEED_SENSING

```

and even the `struct` that holds the sensor reading is defined with maximum memory saving

```c
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
  String led_relayState;
} Data;


```

### Sample Dashboard

Most Functionalities int this site are still experimental
![](./images/test1.png?raw=true)

![](./images/test2.png?raw=true)
