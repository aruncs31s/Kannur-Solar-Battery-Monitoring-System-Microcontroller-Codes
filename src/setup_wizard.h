#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
extern LiquidCrystal_I2C lcd; // Set the LCD address to 0x27 for a 16 chars and 2 line display
extern bool lcd_initialized;
bool scan_for_device();
void setup_lcd();
bool scan_for_device();