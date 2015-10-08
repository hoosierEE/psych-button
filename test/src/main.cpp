// Response button firmware for TeensyLC, built with PlatformIO.
// 
// Implements a modified version of SNTP, where:
//
//      network = Serial-over-USB 
//      server  = host PC
//      client  = this device
// 
// In this modified version, the client and server roles are effectively reversed,
// meaning: the server (instead of the client) keeps track of round-trip delay and offset.
//
// Modified protocol:
//      1. Server sends "initiate timing procedure" request to client
//      2. Client sets an internal stopwatch timer to 0 (T1), and replies with 
//         the number of microseconds that elapsed since it received the message (T2)
//      3. Server notes when it receives reply (T3)
//      4. Server repeats steps 1-3 often enough to be statistically meaningful
#include <SimpleSwitch.h>

// MODEL
const uint8_t NUM_BUTTONS{4};
uint8_t button_pins[NUM_BUTTONS]{2,3,4,5}; // Teensy digital pins
char letters[NUM_BUTTONS]{'w','a','s','d'}; // for keyboard use
SimpleSwitch buttons[NUM_BUTTONS]=
{
    SimpleSwitch(button_pins[0]),
    SimpleSwitch(button_pins[1]),
    SimpleSwitch(button_pins[2]),
    SimpleSwitch(button_pins[3])
};
struct KeyState {
    bool keys[NUM_BUTTONS];
    bool changed;
} ks;

// FUNCTIONS
char boolean_to_hex(struct KeyState *state)
{
    // Send 4 buttons as a hex digit.
    char result{0};
    for (uint8_t i = 0; i < NUM_BUTTONS; ++i) {
        result |= state->keys[i] << i;
    }
    return result;
}

void update_buttons(void)
{
    // read 4 pins and update internal button states
    for (uint8_t i{0}; i < NUM_BUTTONS; ++i) {
        buttons[i].update();
    }
}

void update_serial(void)
{
    // process incoming commands from the Serial buffer, if any
    if (Serial.available()) {
        char command = Serial.read();
        if (command == 'T') {
            // time measurement query response
            uint32_t now{micros()};
            Serial.println(micros() - now); // respond with T3 - T2
        } else if (command == 'L') {
            // keyboard letter replacement
            // e.g. to replace wasd with 1234, send this command over Serial:
            // L1234;
            // Power-cycling will restore default (wasd).
            Serial.readBytesUntil(';',letters,sizeof(letters));
            Serial.print("replaced letters with ");
            Serial.print(letters[0]);
            Serial.print(letters[1]);
            Serial.print(letters[2]);
            Serial.println(letters[3]);
        }
    }
}

void render_output(void)
{
    // send output data to Keyboard or Serial or both
    for (uint8_t i{0}; i < NUM_BUTTONS; ++i) {
        // update internal state
        if (buttons[i].pressed()) {
            ks.changed = true;
            ks.keys[i] = true;

            // send key press (and release) at time of button press
            // Note: key release signal is automatic and not tied to
            // the actual button release.
            Keyboard.write(letters[i]);
        }
        if (buttons[i].released()) {
            ks.changed = true;
            ks.keys[i] = false;
        }

        // Serial output when button state changes (press OR release)
        if (ks.changed) {
            ks.changed = false;
            // send hex-encoded key state ('0' through 'f')
            Serial.println(boolean_to_hex(&ks));
        }
    }
}

void setup() {
    while (!Serial); // do-nothing-wait until Serial is ready
    Serial.begin(115200);
    // Keyboard.begin(); // make sure other stuff works first
}

void loop() {
    // UPDATE
    // Gather input data as fast as possible.
    // Inputs: buttons, Serial
    update_buttons();
    update_serial();

    // RENDER
    // Send data out at a controlled rate.
    // Outputs: Serial, Keyboard
    const uint16_t LOOP_TIME{10000}; // us
    elapsedMicros output_timer; // controls output data rate
    if (output_timer >= LOOP_TIME) {
        output_timer -= LOOP_TIME;
        render_output();
    }
}

