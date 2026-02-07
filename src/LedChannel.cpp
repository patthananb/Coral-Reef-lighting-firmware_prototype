#include "LedChannel.h"

LedChannel::LedChannel(int pin, int channel) {
    _pin = pin;
    _channel = channel;
    _intensity = 0;
}

void LedChannel::attach() {
    ledcAttach(_pin, _pwmFreq, _pwmResolution);
}

void LedChannel::setIntensity(int intensity) {
    _intensity = constrain(intensity, 0, 100);
}

int LedChannel::getIntensity() {
    return _intensity;
}

void LedChannel::write() {
    // Mapping 0-100 to 0-255 for 8-bit resolution
    int dutyCycle = map(_intensity, 0, 100, 0, 255);
    ledcWrite(_channel, dutyCycle);
}
