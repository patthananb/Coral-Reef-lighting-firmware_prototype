#include "Encoder.h"

volatile bool Encoder::_rotateLeftFlag = false;
volatile bool Encoder::_rotateRightFlag = false;
volatile bool Encoder::_buttonPressFlag = false;
Encoder* Encoder::_instance = nullptr;

Encoder::Encoder(uint8_t pinA, uint8_t pinB, uint8_t pinButton, int steps)
    : _rotaryEncoder(pinA, pinB, steps) {
    _instance = this;
    pinMode(pinButton, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(pinButton), buttonISR, FALLING);
}

void Encoder::begin() {
    _rotaryEncoder.begin();
    _rotaryEncoder.setup([]{ Encoder::readEncoderISR(); });
    _rotaryEncoder.setBoundaries(-10000, 100000, true);
    _rotaryEncoder.setAcceleration(0);
}

void IRAM_ATTR Encoder::readEncoderISR() {
    if (_instance) {
        _instance->_rotaryEncoder.readEncoder_ISR();
        long currentPosition = _instance->_rotaryEncoder.readEncoder();
        static long lastPosition = 0;
        if (currentPosition > lastPosition) {
            _rotateRightFlag = true;
        } else if (currentPosition < lastPosition) {
            _rotateLeftFlag = true;
        }
        lastPosition = currentPosition;
    }
}

void IRAM_ATTR Encoder::buttonISR() {
    static unsigned long lastInterruptTime = 0;
    unsigned long interruptTime = millis();
    if (interruptTime - lastInterruptTime > 200) {
        _buttonPressFlag = true;
        lastInterruptTime = interruptTime;
    }
}

bool Encoder::isRotatedLeft() {
    return _rotateLeftFlag;
}

bool Encoder::isRotatedRight() {
    return _rotateRightFlag;
}

bool Encoder::isButtonPressed() {
    return _buttonPressFlag;
}

void Encoder::resetFlags() {
    _rotateLeftFlag = false;
    _rotateRightFlag = false;
    _buttonPressFlag = false;
}
