#ifndef ENCODER_H
#define ENCODER_H

#include <AiEsp32RotaryEncoder.h>

class Encoder {
public:
    Encoder(uint8_t pinA, uint8_t pinB, uint8_t pinButton, int steps);
    void begin();
    void loop();
    bool isRotatedLeft();
    bool isRotatedRight();
    bool isButtonPressed();
    void resetFlags();

private:
    AiEsp32RotaryEncoder _rotaryEncoder;
    static void IRAM_ATTR readEncoderISR();
    static void IRAM_ATTR buttonISR();

    static volatile bool _rotateLeftFlag;
    static volatile bool _rotateRightFlag;
    static volatile bool _buttonPressFlag;

    static Encoder* _instance;
};

#endif // ENCODER_H
