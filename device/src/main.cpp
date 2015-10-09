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
#include <SimpleSwitch.h> // debounces a pin; provides update(), pressed(), released() methods

// MODEL
const uint8_t NUM_BUTTONS{4};
char letters[]{"wasd"}; // default keyboard TODO make sure "string" notation works with array accesses
// buttons connect to TeensyLC pins 2,3,4,5
SimpleSwitch buttons[NUM_BUTTONS]{SimpleSwitch(2),SimpleSwitch(3),SimpleSwitch(4),SimpleSwitch(5)};
struct KeyState {bool keys[NUM_BUTTONS],changed;} ks;

// FUNCTIONS
char bits_to_hex(KeyState *state)
{
    // Convert 4 bits to a single hexadecimal character.
    uint8_t result{0};
    for (uint8_t i{0}; i < NUM_BUTTONS; ++i) { result |= state->keys[i] << i; }
    char hex[] = "0123456789abcdef";
    return hex[result];
}

void update_buttons(void)
{
    // read 4 pins and update internal button states
    for (uint8_t i{0}; i < NUM_BUTTONS; ++i) { buttons[i].update(); }
}

bool valid_letter(char candidate)
{
    // return true only for [A-Za-z0-9]
    if (candidate >= '0' && candidate <= '9') return true;
    if (candidate >= 'A' && candidate <= 'Z') return true;
    if (candidate >= 'a' && candidate <= 'z') return true;
    return false;
}

void customize_keys(void)
{
    // Keyboard letter customization.
    // Power-cycling will restore default (wasd).
    char newLetters[4];
    Serial.readBytes(newLetters,NUM_BUTTONS+1);
    for (uint8_t i{0}; i < NUM_BUTTONS; ++i) {
        letters[i] = valid_letter(newLetters[i]) ? newLetters[i] : letters[i];
    }
    Serial.print("replaced letters with ");
    Serial.println(letters);
    // print_letters();
}

void update_serial(void)
{
    // process incoming commands from the Serial buffer, if any
    // accepts the following 1-letter commands:
    // T (Time)
    // l (list letters)
    // L (change letters)
    if (Serial.available()) {
        uint32_t timeSerialArrived{micros()};
        switch (Serial.read()) {
            case 'T':
                // time measurement query response
                Serial.println(micros() - timeSerialArrived); // respond with T3 - T2
                break;
            case 'L':
                customize_keys();
                break;
            case 'l':
                Serial.println(letters);
                break;
            default:
                break;
        }
        // TODO: test if switch statement properly replaces the section below
        // char command{Serial.read()};
        // if (command == 'T') {
        //     // time measurement query response
        //     Serial.println(micros() - timeSerialArrived); // respond with T3 - T2
        // } else if (command == 'l') {
        //     Serial.println(letters);
        //     // print_letters();
        // } else if (command == 'L') {
        //     customize_keys();
        // }
    }
}

void render_output(void)
{
    // Send data to Keyboard, Serial, or both.
    for (uint8_t i{0}; i < NUM_BUTTONS; ++i) {
        // update internal state
        if (buttons[i].pressed()) {
            ks.changed = true;
            ks.keys[i] = true;

            // Send keyboard letter upon corresponding button press.
            //
            // Note: key RELEASE signal is automatically generated
            // shortly thereafter by the microcontroller, it DOES NOT
            // correspond with when the human releases the button!
            Keyboard.write(letters[i]);
        }
        if (buttons[i].released()) {
            ks.changed = true;
            ks.keys[i] = false;
        }

        // Serial output when button state changes (press OR release)
        if (ks.changed) {
            ks.changed = false; // reset
            // send hex-encoded key state ('0' through 'f')
            Serial.println(bits_to_hex(&ks));
        }
    }
}

// TIMING
const uint16_t LOOP_TIME{10000}; // us
elapsedMicros outputTimer; // controls output data rate

// PROGRAM
void setup() {
    Serial.begin(9600);
    Keyboard.begin(); // make sure other stuff works first
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
    if (outputTimer >= LOOP_TIME) {
        outputTimer -= LOOP_TIME;
        render_output();
    }
}

