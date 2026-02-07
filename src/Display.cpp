#include "Display.h"

Display::Display(uint8_t addr, uint8_t cols, uint8_t rows) : _lcd(addr, cols, rows) {}

void Display::init() {
    _lcd.init();
    _lcd.backlight();
}

void Display::clear() {
    _lcd.clear();
}

void Display::print(const char* text) {
    _lcd.print(text);
}

void Display::setCursor(uint8_t col, uint8_t row) {
    _lcd.setCursor(col, row);
}

void Display::backlight() {
    _lcd.backlight();
}

void Display::showHomepage() {
    _lcd.clear();
    _lcd.setCursor(0, 0);
    _lcd.print("HOMEPAGE");
    _lcd.setCursor(0, 1);
    _lcd.print("Time: 13:00");
}

void Display::showSelectChannel() {
    _lcd.clear();
    _lcd.setCursor(0, 0);
    _lcd.print("SETTING");
    _lcd.setCursor(0, 1);
    _lcd.print(">> BRIGHTNESS");
}

void Display::showOnoffTimer() {
    _lcd.clear();
    _lcd.setCursor(0, 0);
    _lcd.print("SETTING");
    _lcd.setCursor(0, 1);
    _lcd.print(">> TIMER");
}

void Display::showSelectedChannel(int channel) {
    _lcd.clear();
    _lcd.setCursor(0, 0);
    _lcd.print("SELECT");
    _lcd.setCursor(0, 1);
    _lcd.print("Channel ");
    _lcd.print(channel);
}

void Display::showSelectedMasterVolume() {
    _lcd.clear();
    _lcd.setCursor(0, 0);
    _lcd.print("SELECT");
    _lcd.setCursor(0, 1);
    _lcd.print("Master Volume");
}

void Display::showAdjustIntensity(int value) {
    _lcd.setCursor(1, 0);
    _lcd.print("Intensity: ");
    _lcd.setCursor(12, 0);
    _lcd.print("   ");
    _lcd.setCursor(12, 0);
    _lcd.print(value);

    int barLength = map(value, 0, 100, 0, 16);
    _lcd.setCursor(0, 1);
    for (int i = 0; i < 16; i++) {
        if (i < barLength) {
            _lcd.write(byte(255));
        } else {
            _lcd.print(" ");
        }
    }
}
