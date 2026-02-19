#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
LiquidCrystal_I2C lcd(0x27, 16, 2); // Set the LCD address to 0x27 for a 16 chars and 2 line display

bool scan_for_device();
void setup_lcd()
{
    // Does the Wire.begin()
    lcd.init(); // initialize the lcd

    if (!scan_for_device()) {
        delete &lcd;
        return ;
    }

    lcd.backlight(); // open the backlight

    lcd.setCursor(0, 0);
    lcd.print("SKVMS Solar");
    lcd.setCursor(0, 1);
    lcd.print("Monitor");

    pinMode(36, INPUT);
    // volatile int *ptr = (volatile int *) 0x3FF40004; // ADC1_CTRL_REG
    int adcVal = 0;
    // ADC Resolution is 12 bits, so max value is 4095
    analogReadResolution(12);

    while (1)
    {
        adcVal = analogRead(36);
        lcd.setCursor(0, 1);
        lcd.print("ADC: " + String(adcVal) + "   ");
        // Serial.println("ADC Value: " + String(adcVal));
        delay(1000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("SKVMS Solar");
    }
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