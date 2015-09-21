//
// main.cpp
//
// 4-button response box with USB-Serial timestamps
// For accurate subject timing, initialize with "start" command (using USB Serial)
//

#include "../lib/SimpleSwitch/SimpleSwitch.h"

// HARDWARE
const uint8_t NUM_SWITCHES{4};
SimpleSwitch s0(0);
SimpleSwitch s1(1);
SimpleSwitch s2(2);
SimpleSwitch s3(3);

// MODELS
uint32_t t0;
SimpleSwitch switches[NUM_SWITCHES]{s0,s1,s2,s3};
const char letters[NUM_SWITCHES]{'a','b','c','d'}; // friendly names for buttons
struct KeyState {
    bool keys[NUM_SWITCHES];
    bool prev[NUM_SWITCHES];
    bool changed;
};
struct KeyState ks;

void resetTimer(void)
{
    // TODO this thing
    if (Serial.available()) {
        char inbyte = Serial.read();
        if (inbyte == '0') {
            t0 = 0;
        }
    }
}

void serialize(uint32_t now, struct KeyState *state)
{
    // Send all the buttons (in hex) and a timestamp (in decimal)
    char result{0};
    for (uint8_t i = 0; i < NUM_SWITCHES; ++i) {
        result |= state->keys[i] << i;
    }
    Serial.print(result, HEX); // '0' through 'F'
    Serial.print(" "); // easier for human to read
    Serial.println(now, HEX); // hex is about 12% faster on average
}

void setup() {
    Serial.begin(115200);
    Keyboard.begin();
}

void loop() {
    // UPDATE
    uint32_t now = micros();
    for (uint8_t i = 0; i < NUM_SWITCHES; ++i) {
        switches[i].update(now);
        ks.keys[i] = switches[i].pressed();
        if (ks.prev[i] != ks.keys[i]) {
            Keyboard.write(letters[i]); // do this the instant they arrive
            ks.changed = true;
        }
    }

    // RENDER
    // if there were any changes, send an update
    if (ks.changed) {
        serialize(now, &ks); // send these in a batch
        for (uint8_t i = 0; i < NUM_SWITCHES; ++i) {
            ks.prev[i] = ks.keys[i]; // make previous same as current
        }
        ks.changed = false; // reset so we don't send the same update twice
    }
}

