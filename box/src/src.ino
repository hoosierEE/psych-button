// 4-button response box with USB-Serial timestamps
// For accurate subject timing, initialize with "start" command (using USB Serial)
// And listen for incoming events, which will have this format:
// (s0 s1 s2 s3 timestamp)
// ^                     ^ Note each message is delimited by an open and closed parenthesis.
// Client applications may use this for rudimentary error-checking.

#include "../lib/SimpleSwitch/SimpleSwitch.h"

// HARDWARE
const uint8_t NUM_SWITCHES{4};
SimpleSwitch s0(0);
SimpleSwitch s1(1);
SimpleSwitch s2(2);
SimpleSwitch s3(3);
SimpleSwitch switches[NUM_SWITCHES]{s0,s1,s2,s3};

const char letters[NUM_SWITCHES]{'a','b','c','d'}; // friendly names for buttons

// MODEL
typedef struct {
    bool keys[NUM_SWITCHES];
    bool prev[NUM_SWITCHES]; // used to determine changes
    bool changed;
} KeyState;
KeyState key;

// RENDER
void serialize(uint32_t now, KeyState state)
{
    // Send all the buttons (in hex) and a timestamp (in decimal)
    char result{0};
    for (uint8_t i = 0; i < NUM_SWITCHES; ++i) {
        result |= state.keys[i] << i;
    }
    Serial.print(result, HEX); // '0' through 'F'
    Serial.print(" "); // easier for human to read
    Serial.println(now);
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
        key.keys[i] = switches[i].pressed();
        if (key.prev[i] != key.keys[i]) {
            Keyboard.write(letters[i]); // do this the instant they arrive
            key.changed = true;
        }
    }

    // if there were any changes, send an update
    if (key.changed) {
        serialize(now, key); // send these in a batch
        for (uint8_t i = 0; i < NUM_SWITCHES; ++i) {
            key.prev[i] = key.keys[i]; // make previous same as current
        }
        key.changed = false; // reset so we don't send the same update twice
    }
}

