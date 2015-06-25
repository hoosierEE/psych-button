// 4-button response box with USB-Serial timestamps
// For accurate subject timing, initialize with "start" command (using USB Serial)
// And listen for incoming events, which will have this format:
// (s0 s1 s2 s3 timestamp)
// ^                     ^ Note each message is delimited by an open and closed parenthesis.

#include "../lib/SimpleSwitch/SimpleSwitch.h"

// switches
const uint8_t NUM_SWITCHES{4};
SimpleSwitch s0(0);
SimpleSwitch s1(1);
SimpleSwitch s2(2);
SimpleSwitch s3(3);
SimpleSwitch switches[NUM_SWITCHES]{s0,s1,s2,s3};

// switch states
typedef struct {
    bool keys[NUM_SWITCHES];
    bool prev[NUM_SWITCHES];
    bool changed;
} KeyState;
KeyState k;

void render(uint32_t now)
{
    // print the switch states
    Serial.print("(");
    for (uint8_t i = 0; i < NUM_SWITCHES; ++i) {
        Serial.print(k.keys[i]);
        Serial.print(", ");
    }
    Serial.print(now); // print the time
    Serial.println(")"); // delimiter and newline
}

void setup() {
    Serial.begin(115200);
}

void loop() {
    // update timers
    uint32_t now = micros(); // global time for this loop

    // update inputs
    for (uint8_t i = 0; i < NUM_SWITCHES; ++i) {
        switches[i].update(now); // synchronized update for all switches
    }

    // check for changes
    for (uint8_t i = 0; i < NUM_SWITCHES; ++i) {
        k.keys[i] = switches[i].pressed(); // removes a press from the queue
        if (k.prev[i] != k.keys[i]) {
            k.changed |= true; // set a flag if any of the inputs changed
        }
    }

    // if there were any changes, send an update
    if (k.changed)
        render(now);
}

