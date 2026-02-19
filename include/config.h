#ifndef __CONFIG_H_
#define __CONFIG_H_
#include <Arduino.h>
#include <cstdint>

#define RETRY_COUNT 59999

#define DEBUG


// Here every features should be defined

#define ONEPLUS_AP
// #define LIGHT_SENSING
// #define HUMID_TEMP_SENSING
// #define STATIC_IP
// #define LCD_DSPLAY
// #define RAIN_SENSING
// #define WIND_DIRECTION_SENSING
// #define WIND_SPEED_SENSING

// Go Backend Configuration
#define USE_GO_BACKEND
#define BACKEND_HOST "10.71.76.103"
#define BACKEND_PORT 8080
#define DEVICE_NAME "ESP_32_DEV_1"
#define SEND_INTERVAL 20000 // Send data every 10 seconds
#define TOKEN "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VyX2lkIjoyLCJkZXZpY2VfaWQiOjEyLCJzdWIiOiJkZXZpY2U6MTI6dXNlcjoyIiwiZXhwIjoxNzc4NzAzMzY1LCJuYmYiOjE3NzAwNjMzNjUsImlhdCI6MTc3MDA2MzM2NX0.s7fXfwTi4uLWjOunvipfJk9XCYx_sVtEXadFJYx8yZQ"

// Wifi Credentials

#if defined(ONEPLUS_AP)
#define WIFI_SSID "pi_wifi"
#define WIFI_PASSWORD "12345678"

#if defined(STATIC_IP)
#define STATIC_IP_ADDRESS 192, 168, 72, 50
#define STATIC_IP_GATEWAY 192, 168, 72, 1
#define STATIC_IP_SUBNET 255, 255, 255, 0
#endif
#endif

#if defined(GCEK_AP) && defined(STATIC_IP)
#define WIFI_SSID "GCEK-WiFi"
#define WIFI_PASSWORD ""
#define STATIC_IP_ADDRESS 172, 16, 32, 9
#define STATIC_IP_GATEWAY 172, 16, 32, 1
#define STATIC_IP_SUBNET 255, 255, 252, 0
#endif

// Pin Configuration

#if defined(ESP8266)
const uint8_t led_relayPin = D2;
#elif defined(ESP32)
const uint8_t led_relayPin = 4;
#endif

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

// For web Server

#define TIMEOUT_MS 2000

// Battery Voltage Meter
// #define R1 1  // 1K
// #define R2 10 // 10K
#define R1 1
#define R2 4.7
#define V_REF 3.3
#if defined(ESP8266)
const uint8_t battery_voltage_pin = A0;
#elif defined(ESP32)
const uint8_t battery_voltage_pin = 36;
#endif
const int PIN_WIND_DIRECTION = 32;
const int PIN_WIND_SPEED = 34;
const int PIN_RAINFALL = 35;

#define BAUD_RATE 115200

#if defined(ESP8266)
#define ADC_MAX 1023
#define ADC_OFFSET 0
#elif defined(ESP32)
#define ADC_MAX 4095
#define ADC_OFFSET 134
#else
#error                                                                         \
    "Please define which the max of your adc 1023 for 10 bit adc and 4095 for 12 bit adc"
#endif

/* ###### END :) ########*/
#endif //!__CONFIG_H_
