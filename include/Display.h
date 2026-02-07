#ifndef DISPLAY_H
#define DISPLAY_H

#include <LiquidCrystal_I2C.h>

class Display {
public:
    Display(uint8_t addr, uint8_t cols, uint8_t rows);
    void init();
    void clear();
    void print(const char* text);
    void setCursor(uint8_t col, uint8_t row);
    void backlight();

    void showHomepage();
    void showSelectChannel();
    void showOnoffTimer();
    void showSelectedChannel(int channel);
    void showSelectedMasterVolume();
    void showAdjustIntensity(int value);


private:
    LiquidCrystal_I2C _lcd;
};

#endif // DISPLAY_H
