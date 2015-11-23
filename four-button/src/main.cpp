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
const uint16_t LOOP_TIME{10000}; // value in microseconds
elapsedMicros outputTimer; // determines output data rate

// MODEL
const uint8_t NUM_BUTTONS{4};
char l[]{"wxyz"}; // default keyboard TODO make sure "string" notation works with array accesses
char h[]{"_-."}; // home button down(_), up(-), or no change(.)
struct KeyState {bool keys[NUM_BUTTONS],changed; uint8_t homebutton;} ks;

// HARDWARE CONNECTIONS
SimpleSwitch buttons[NUM_BUTTONS]{SimpleSwitch(2),SimpleSwitch(3),SimpleSwitch(4),SimpleSwitch(5)}; // mech. switches
CapSwitch cap(15); // capacitive 'switch' at the given pin

// FUNCTIONS
bool valid_letter(char c){return ((c>='0'&&c<='9')||(c>='A'&&c<='Z')||(c>='a'&&c<='z'));} // true for [09AZaz]

// I/O
void update_buttons(void){DO(NUM_BUTTONS){buttons[i].update();}} // update buttons with pin readings
void update_touch(void) {cap.update();}

void customize_keys(void)
{
    // Keyboard letter customization.
    // Power-cycling restores default (wxyz).
    char nl[4]; // new letters
    Serial.readBytes(nl,NUM_BUTTONS+1);
    DO(NUM_BUTTONS){l[i] = valid_letter(nl[i]) ? nl[i] : l[i];}
    Serial.print("replaced letters with ");
    Serial.println(l);
}

void read_serial(void)
{
    // process incoming commands from the Serial buffer, if any
    // accepts the following 1-letter commands:
    // T (Time)
    // l (list letters)
    // L (change letters)
    uint32_t t2{micros()};
    if (Serial.available()) {
        switch (Serial.read()) {
        case 'T': Serial.println(micros() - t2); break; // respond with T3 - T2
        case 'L': customize_keys(); break;
        case 'l': Serial.println(l); break;
        default: break;
        }
    }
}

void render_output(KeyState *k) // TODO: refactor so this is truly output-only
{
    // Update the data (KeyState)
    DO(NUM_BUTTONS) {
        if (buttons[i].pressed()) {
            k->changed = true;
            k->keys[i] = true;
            // Send keyboard letter with corresponding button press.
            Keyboard.press(l[i]);
            Serial.println(cap.get_raw());
        }
        if (buttons[i].released()) {
            k->changed = true;
            k->keys[i] = false;
            // Release keyboard letter with corresponding button release.
            Keyboard.release(l[i]);
        }
    }

    k->homebutton = 2; // assume no change
    if (cap.pressed()) {
        k->homebutton = 0;
        k->changed = true;
    } else if (cap.released()) {
        k->homebutton = 1;
        k->changed = true;
    }

    // print if changed
    if (k->changed) {
        k->changed = false; // reset
        const uint8_t BLEN{11};
        char buf[BLEN]={0}; // holds an array of null terminators
        snprintf(buf,BLEN,"%d %d %d %d %c\n",
                 k->keys[0],k->keys[1],k->keys[2],k->keys[3],h[k->homebutton]);
        Serial.print(buf);
    }
}

// PROGRAM
void setup() {
    Serial.begin(9600);
    Keyboard.begin();
}

void loop() {
    // UPDATE
    // Gather input data as fast as possible.
    // Inputs: buttons, Serial
    update_buttons();
    update_touch();
    read_serial();

    // RENDER
    // Send data out at a controlled rate.
    // Outputs: Serial, Keyboard
    if (outputTimer >= LOOP_TIME) {
        outputTimer -= LOOP_TIME;
        render_output(&ks);
    }
}
