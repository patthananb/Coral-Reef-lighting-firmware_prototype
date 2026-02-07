#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <EEPROM.h>
#include "Display.h"
#include "Encoder.h"
#include "LedChannel.h"

#define ROTARY_ENCODER_A_PIN 34
#define ROTARY_ENCODER_B_PIN 35
#define ROTARY_ENCODER_BUTTON_PIN 32
#define ROTARY_ENCODER_STEPS 1

#define PWM_FAN 14 // GPIO 14 for Fan PWM

// EEPROM addresses
const int address_Intensity0 = 1;
const int address_Intensity1 = 2;
const int address_Intensity2 = 3;
const int address_Intensity3 = 4;
const int address_Intensity4 = 5;
const int address_masterVolume = 6;

const int ledPins[] = {33, 27, 12, 26, 25};
const int numChannels = 5;
LedChannel* ledChannels[numChannels];

Display display(0x27, 16, 2);
Encoder encoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_STEPS);

int pageIndex = 0;
int masterVolume = 100;

enum State
{
    HOME,
    MODE_SELECT,
    SELECT_CHANNEL_MODE,
    ON_OFF_TIMER_MODE,
    ADJUST_INTENSITY_MASTER_VOLUME,
    ADJUST_INTENSITY_CH1_MODE,
    ADJUST_INTENSITY_CH2_MODE,
    ADJUST_INTENSITY_CH3_MODE,
    ADJUST_INTENSITY_CH4_MODE,
    ADJUST_INTENSITY_CH5_MODE,
};

State currentState = HOME;

void EEPROM_SET();
void applyMasterVolume();
void writeAllChannels();

void setup()
{
    Serial.begin(115200);
    pinMode(PWM_FAN, OUTPUT);
    digitalWrite(PWM_FAN, HIGH); // Turn fan on

    display.init();
    display.setCursor(0, 0);
    display.print("WELCOME");
    display.setCursor(0, 1);
    display.print("Initializing...");
    delay(500);

    encoder.begin();

    for (int i = 0; i < numChannels; i++) {
        ledChannels[i] = new LedChannel(ledPins[i], i);
        ledChannels[i]->attach();
    }

    EEPROM_SET();
    applyMasterVolume();
    writeAllChannels();

    currentState = HOME;
}

void loop()
{
    switch (currentState)
    {
    case HOME:
    {
        display.showHomepage();
        if (encoder.isButtonPressed())
        {
            currentState = MODE_SELECT;
            encoder.resetFlags();
        }
    }
    break;
    case MODE_SELECT:
    {
        State currentStateTemp = SELECT_CHANNEL_MODE;
        unsigned long previousTime = millis();
        display.showSelectChannel();

        while (true)
        {
            if (encoder.isRotatedRight() || encoder.isRotatedLeft())
            {
                pageIndex++;
                if (pageIndex % 2 == 1)
                {
                    display.showSelectChannel();
                    currentStateTemp = SELECT_CHANNEL_MODE;
                }
                else
                {
                    display.showOnoffTimer();
                    currentStateTemp = ON_OFF_TIMER_MODE;
                }
                previousTime = millis();
                encoder.resetFlags();
            }
            else if (encoder.isButtonPressed())
            {
                currentState = currentStateTemp;
                pageIndex = 0;
                encoder.resetFlags();
                break;
            }
            else if (millis() - previousTime > 3000)
            {
                currentState = HOME;
                pageIndex = 0;
                encoder.resetFlags();
                break;
            }
        }
    }
    break;
    case SELECT_CHANNEL_MODE:
    {
        State currentStateTemp1 = ADJUST_INTENSITY_CH1_MODE;
        unsigned long previousTime = millis();
        display.showSelectedChannel(1);

        while (true)
        {
            if (encoder.isRotatedRight())
            {
                pageIndex++;
                if (pageIndex % 6 == 0) { display.showSelectedChannel(1); currentStateTemp1 = ADJUST_INTENSITY_CH1_MODE; }
                else if (pageIndex % 6 == 1) { display.showSelectedChannel(2); currentStateTemp1 = ADJUST_INTENSITY_CH2_MODE; }
                else if (pageIndex % 6 == 2) { display.showSelectedChannel(3); currentStateTemp1 = ADJUST_INTENSITY_CH3_MODE; }
                else if (pageIndex % 6 == 3) { display.showSelectedChannel(4); currentStateTemp1 = ADJUST_INTENSITY_CH4_MODE; }
                else if (pageIndex % 6 == 4) { display.showSelectedChannel(5); currentStateTemp1 = ADJUST_INTENSITY_CH5_MODE; }
                else if (pageIndex % 6 == 5) { display.showSelectedMasterVolume(); currentStateTemp1 = ADJUST_INTENSITY_MASTER_VOLUME; }
                previousTime = millis();
                encoder.resetFlags();
            }
            else if (encoder.isRotatedLeft())
            {
                pageIndex--;
                if (pageIndex < 0) pageIndex = 5;
                if (pageIndex % 6 == 0) { display.showSelectedChannel(1); currentStateTemp1 = ADJUST_INTENSITY_CH1_MODE; }
                else if (pageIndex % 6 == 1) { display.showSelectedChannel(2); currentStateTemp1 = ADJUST_INTENSITY_CH2_MODE; }
                else if (pageIndex % 6 == 2) { display.showSelectedChannel(3); currentStateTemp1 = ADJUST_INTENSITY_CH3_MODE; }
                else if (pageIndex % 6 == 3) { display.showSelectedChannel(4); currentStateTemp1 = ADJUST_INTENSITY_CH4_MODE; }
                else if (pageIndex % 6 == 4) { display.showSelectedChannel(5); currentStateTemp1 = ADJUST_INTENSITY_CH5_MODE; }
                else if (pageIndex % 6 == 5) { display.showSelectedMasterVolume(); currentStateTemp1 = ADJUST_INTENSITY_MASTER_VOLUME; }
                previousTime = millis();
                encoder.resetFlags();
            }
            else if (encoder.isButtonPressed())
            {
                currentState = currentStateTemp1;
                encoder.resetFlags();
                break;
            }
            else if (millis() - previousTime > 3000)
            {
                currentState = HOME;
                pageIndex = 0;
                encoder.resetFlags();
                break;
            }
        }
    }
    break;
    case ADJUST_INTENSITY_CH1_MODE:
    case ADJUST_INTENSITY_CH2_MODE:
    case ADJUST_INTENSITY_CH3_MODE:
    case ADJUST_INTENSITY_CH4_MODE:
    case ADJUST_INTENSITY_CH5_MODE:
    {
        int channelIndex = currentState - ADJUST_INTENSITY_CH1_MODE;
        int pwmValue = ledChannels[channelIndex]->getIntensity();
        display.clear();

        while (true)
        {
            pwmValue = constrain(pwmValue, 0, 100);
            if (encoder.isRotatedRight())
            {
                pwmValue++;
                encoder.resetFlags();
            }
            else if (encoder.isRotatedLeft())
            {
                pwmValue--;
                encoder.resetFlags();
            }
            else if (encoder.isButtonPressed())
            {
                ledChannels[channelIndex]->setIntensity(pwmValue);
                EEPROM.write(address_Intensity0 + channelIndex, pwmValue);
                EEPROM.commit();
                applyMasterVolume();
                writeAllChannels();
                currentState = SELECT_CHANNEL_MODE;
                encoder.resetFlags();
                break;
            }
            display.showAdjustIntensity(pwmValue);
            // Temporary write for preview
            int tempDutyCycle = map(pwmValue * masterVolume / 100, 0, 100, 0, 255);
            ledcWrite(channelIndex, tempDutyCycle);
        }
    }
    break;
    case ADJUST_INTENSITY_MASTER_VOLUME:
    {
        int tempMasterVolume = masterVolume;
        display.clear();
        while (true)
        {
            tempMasterVolume = constrain(tempMasterVolume, 0, 100);
            if (encoder.isRotatedRight())
            {
                tempMasterVolume++;
                encoder.resetFlags();
            }
            else if (encoder.isRotatedLeft())
            {
                tempMasterVolume--;
                encoder.resetFlags();
            }
            else if (encoder.isButtonPressed())
            {
                masterVolume = tempMasterVolume;
                EEPROM.write(address_masterVolume, masterVolume);
                EEPROM.commit();
                applyMasterVolume();
                writeAllChannels();
                currentState = SELECT_CHANNEL_MODE;
                encoder.resetFlags();
                break;
            }
            display.showAdjustIntensity(tempMasterVolume);
        }
    }
    break;
    case ON_OFF_TIMER_MODE:
    {
        // Placeholder for timer functionality
        currentState = HOME;
        encoder.resetFlags();
    }
    break;
    }
    delay(100);
}

void EEPROM_SET()
{
    EEPROM.begin(20);
    for (int i = 0; i < numChannels; i++) {
        ledChannels[i]->setIntensity(EEPROM.read(address_Intensity0 + i));
    }
    masterVolume = EEPROM.read(address_masterVolume);
    if (masterVolume == 0) masterVolume = 100; // Default to 100 if not set
}

void applyMasterVolume() {
    // This logic is now integrated into the write functions
}

void writeAllChannels() {
    for (int i = 0; i < numChannels; i++) {
        int finalIntensity = ledChannels[i]->getIntensity() * masterVolume / 100;
        int dutyCycle = map(finalIntensity, 0, 100, 0, 255);
        ledcWrite(i, dutyCycle);
    }
}
