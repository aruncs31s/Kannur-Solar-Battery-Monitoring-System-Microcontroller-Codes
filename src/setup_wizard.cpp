#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
LiquidCrystal_I2C lcd(0x27, 16, 2); // Set the LCD address to 0x27 for a 16 chars and 2 line display


bool lcd_initialized = false;
bool scan_for_device();
void setup_lcd()
{
    // Does the Wire.begin()
    lcd.init(); // initialize the lcd

    // if (!scan_for_device()) {
    //     delete &lcd;
    //     return ;
    // }

    lcd_initialized = true;
    lcd.backlight(); // open the backlight

    lcd.setCursor(0, 0);
    lcd.print("SKVMS Solar");
    lcd.setCursor(0, 1);
    lcd.print("Monitor");
    delay(2000);
}
bool scan_for_device()
{
    byte error, address;
    bool devicesFound = false;
    int16_t deviceCount = 0;
    Serial.println("Scanning...");

    for (address = 1; address < 127; address++)
    {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0)
        {
            Serial.print("Device found at address: 0x");
            if (address < 16)
                Serial.print("0");
            Serial.println(address, HEX);
            deviceCount++;
            devicesFound = true;
        }
    }
}