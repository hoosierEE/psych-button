// Firmware for TeensyLC, built with PlatformIO.
//
// Implements a USB response box with 1ms timing resolution.
// To ensure 1ms resolution, latency information is part of 
// the transmitted data.
//
// The other data can be whatever sort of responses you need,
// in this case it's a set of pushbuttons.
#include <Arduino.h>
#include <Bounce.h>

// CONSTANTS
const uint8_t NUM_BUTTONS{4};
const uint8_t DEBOUNCE_TIME{100};

// GLOBAL OBJECTS
elapsedMicros timer;
uint8_t button_pins[NUM_BUTTONS] = { 2, 3, 4, 5 };
Bounce buttons[NUM_BUTTONS] =
{
    Bounce(button_pins[0],DEBOUNCE_TIME),
    Bounce(button_pins[1],DEBOUNCE_TIME),
    Bounce(button_pins[2],DEBOUNCE_TIME),
    Bounce(button_pins[3],DEBOUNCE_TIME)
};

// PROGRAM
void setup(){
    for (uint8_t i = 0; i < NUM_BUTTONS; ++i) {
        // initialize buttons
        pinMode(button_pins[i], INPUT_PULLUP);
    }
}

void loop(){
    // from http://alexshroyer.com/Synchronized-Devices/
    //
    // 1. Accept 'begin' signal (and system time) from host PC.
    // 2. Respond by setting uC clock to 0.
    // 3. Emit data with time == 0 + delta_time.
    // 4. PC responds with time it received data.
    // 5. Subsequent data includes round-trip latency.

    // UPDATE
    // gather input data at short intervals
    
    // RENDER
    // send output data at longer intervals
}

