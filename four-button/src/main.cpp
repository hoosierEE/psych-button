// Response button firmware for TeensyLC, built with PlatformIO.
//
// Implements a modified SNTP timing protocol,
// where 'server' is a PC and 'client' is this device:
// 1. Server sends "initiate timing procedure" request to client
// 2. Client sets an internal stopwatch timer to 0 (T1), and replies with
//    the number of microseconds that elapsed since it received the message (T2).
//    Note that this is the same value as (T2-T1).
// 3. Server notes when it receives reply (T3)
// 4. Server repeats steps 1-3 often enough to be statistically meaningful

#include <SimpleSwitch.h> // debounce a pin
#include <CapSwitch.h> // capacitive sensor as-a-switch

// perform loop body n times, with capture of loop index i
#define DO(n) for(int i=0,_n=(n); i<_n;++i)

// TIMING
const uint16_t LOOP_TIME{10000}; // in microseconds
elapsedMicros outputTimer; // data rate

// MODEL
const uint8_t NUM_BUTTONS{4};
char l[]{"wxyz"}; // letter defaults
char h[]{"_"}; // home button key '_'
struct KeyState { bool keys[NUM_BUTTONS],ch,hb; } ks; // { keys[], changed, home button }

// HARDWARE CONNECTIONS
SimpleSwitch buttons[NUM_BUTTONS]{
    SimpleSwitch(2),SimpleSwitch(3),SimpleSwitch(4),SimpleSwitch(5) }; // mech. switches
CapSwitch cap(23); // capacitive 'switch' at this pin

bool alnum(char c) { return ((c>='0'&&c<='9')||(c>='A'&&c<='Z')||(c>='a'&&c<='z')); } // true for [09AZaz]
void ub(void) { DO(NUM_BUTTONS){buttons[i].update();} } // update buttons with pin readings
void ut(void) { cap.update(); } // update touch-sensitive "button"
void cl(void) // change letters used by Keyboard
{
    // Note: Power-cycling restores default (wxyz).
    char nl[NUM_BUTTONS]; // new letters
    Serial.readBytes(nl,NUM_BUTTONS+1); // utilizes Serial.setTimeout() default (1000ms)
    DO(NUM_BUTTONS){ l[i] = alnum(nl[i]) ? nl[i] : l[i]; }
    Serial.print("replaced letters with ");
    Serial.println(l);
}

void sr(void)  // read the serial port
{
    // Consume and process at most 1 command from the Serial buffer.
    uint32_t t2{micros()};
    if (Serial.available()) {
        switch (Serial.read()) {
        case 'T': Serial.println(micros() - t2); break; // Time: T3 - T2
        case 'L': cl(); break; // L (change letters)
        case 'l': Serial.println(l); break; // l (list letters)
        default: break;
        }
    }
}

KeyState upks(KeyState k) // update key states
{
    // k.ch = false; // assume no change
    if (cap.pressed()) { k.ch = true; k.hb = true; }
    if (cap.released()) { k.ch = true; k.hb = false; }
    DO(NUM_BUTTONS) {
        if (buttons[i].pressed()) { k.ch = true; k.keys[i] = true; }
        if (buttons[i].released()) { k.ch = true; k.keys[i] = false; }
    }
    return k;
}

void rk(const KeyState &kn, const KeyState &ko) // render keyboard
{
    // 'press' if old was low and new is high
    // 'release' if old was high and new is low
    DO(NUM_BUTTONS) {
        if (kn.keys[i] && !ko.keys[i]) Keyboard.press(l[i]);
        if (!kn.keys[i] && ko.keys[i]) Keyboard.release(l[i]);
    }
    if (kn.hb && !ko.hb) Keyboard.press(h[0]);
    if (!kn.hb && ko.hb) Keyboard.release(h[0]);
    // It's possible this could be expressed more simply as:
    // kn.keys[i] ? Keyboard.press(l[i]) : Keyboard.release(l[i])
    // But it strikes me that a real-world key must be released before
    // it can be pressed again.  Maybe OOP is getting to me...
}

void rs(const KeyState &k) // render (print) serial
{
    DO(NUM_BUTTONS) {
        Serial.print(k.keys[i]); // 0 or 1
        Serial.print(" ");
    }
    Serial.println(k.hb); // 0 or 1
}

// PROGRAM
void setup() {
    Serial.begin(9600);
    Keyboard.begin();
}

void loop() {
    // Update "inputs" at maximum rate:
    // buttons, touch, Serial
    ub(); ut(); sr();

    // Update state and render output at a controlled rate.
    if (outputTimer >= LOOP_TIME) {
        outputTimer -= LOOP_TIME;
        // UPDATE
        KeyState oldks = ks; // save current state
        ks = upks(ks); // get new state
        // RENDER
        if (ks.ch) {
            ks.ch = false;
            rs(ks);
            rk(ks,oldks);
        }
    }
    // Alternatively, UPDATE and RENDER could be done at different rates:
    // // UPDATE
    // if (stateTimer >= STATE_TIME) {
    //     stateTimer -= STATE_TIME;
    //     upks(ks); // Update at one rate...
    // }
    // // RENDER
    // if (renderTimer >= RENDER_TIME) {
    //     renderTimer -= RENDER_TIME;
    //     rs(ks); // ...output at other rate.
    // }
}
