#ifndef LEDCHANNEL_H
#define LEDCHANNEL_H

#include <Arduino.h>

class LedChannel {
public:
    LedChannel(int pin, int channel);
    void attach();
    void setIntensity(int intensity); // 0-100
    int getIntensity();
    void write();

private:
    int _pin;
    int _channel;
    int _intensity; // 0-100
    const int _pwmFreq = 20000;
    const int _pwmResolution = 8;
};

#endif // LEDCHANNEL_H
