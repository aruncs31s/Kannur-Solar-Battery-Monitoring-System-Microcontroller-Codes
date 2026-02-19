#include <Arduino.h>
#include "config.h"
#include "initializer.h"


void init_serial() {
  #if defined(DEBUG)
  Serial.begin(BAUD_RATE);
  Serial.println("\n\n========== SKVMS ESP8266 Starting ==========");
  #endif
}
