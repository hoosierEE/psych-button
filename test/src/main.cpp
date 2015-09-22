
// Use more of Teensyduino's built-in goodies

#include <Arduino.h>
#include <Bounce.h>

Bounce buttons[4] = { Bounce(12,100), Bounce(13,100), Bounce(14,100), Bounce(15,100) };

void setup(){
    for (uint8_t i = 0; i < sizeof(buttons); ++i) {
        pinMode(i + 12, INPUT_PULLUP);
    }
}

void loop(){}

