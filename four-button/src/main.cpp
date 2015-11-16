// Response button firmware for TeensyLC, built with PlatformIO.
//
// Modified SNTP timing protocol, where 'server' is a PC and 'client' is this device
//      1. Server sends "initiate timing procedure" request to client
//      2. Client sets an internal stopwatch timer to 0 (T1), and replies with
//         the number of microseconds that elapsed since it received the message (T2).
//         Note that this is the same value as (T2-T1).
//      3. Server notes when it receives reply (T3)
//      4. Server repeats steps 1-3 often enough to be statistically meaningful

#include <SimpleSwitch.h> // debounces a pin; provides update(), pressed(), released() methods
// "Do" the loop body `n` times, with capture of loop index `i` as local variable.
#define DO(n) for(int i=0,_n=(n); i<_n;++i)

// MODEL
const uint8_t NUM_BUTTONS{4};
char letters[]{"wxyz"}; // default keyboard TODO make sure "string" notation works with array accesses
// buttons connect to TeensyLC pins 2,3,4,5
SimpleSwitch buttons[NUM_BUTTONS]{SimpleSwitch(2),SimpleSwitch(3),SimpleSwitch(4),SimpleSwitch(5)};
struct KeyState {bool keys[NUM_BUTTONS],changed;} ks;

// FUNCTIONS
char bits_to_hex(KeyState *state)
{
	// Convert 4 bits to a single hexadecimal character.
	uint8_t result{0};
	DO(NUM_BUTTONS) { result |= state->keys[i] << i; }
	char hex[]{"0123456789abcdef"};
	return hex[result];
}

void update_buttons(void)
{
	// read 4 pins and update internal button states
	DO(NUM_BUTTONS) { buttons[i].update(); }
}

bool valid_letter(char c)
{
	// return true only for [A-Za-z0-9]
	if (c >= '0' && c <= '9') return true;
	if (c >= 'A' && c <= 'Z') return true;
	if (c >= 'a' && c <= 'z') return true;
	return false;
}

void customize_keys(void)
{
	// Keyboard letter customization.
	// Power-cycling restores default (wxyz).
	char newLetters[4];
	Serial.readBytes(newLetters,NUM_BUTTONS+1);
	DO(NUM_BUTTONS) {
		letters[i] = valid_letter(newLetters[i]) ? newLetters[i] : letters[i];
	}
	Serial.print("replaced letters with ");
	Serial.println(letters);
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
	}
}

void render_output(KeyState *keystate)
{
	// Send data to Keyboard, Serial, or both.
	DO(NUM_BUTTONS) {
		// update internal state
		if (buttons[i].pressed()) {
			keystate->changed = true;
			keystate->keys[i] = true;
			// Send keyboard letter with corresponding button press.
			Keyboard.press(letters[i]);
		}
		if (buttons[i].released()) {
			keystate->changed = true;
			keystate->keys[i] = false;
			// Release keyboard letter with corresponding button release.
			Keyboard.release(letters[i]);
		}

		// Serial output when button state changes (press OR release)
		if (keystate->changed) {
			keystate->changed = false; // reset
			// send hex-encoded key state ('0' through 'f')
			//Serial.println(bits_to_hex(&keystate)); // 1:4 compression... worth it?
			DO(NUM_BUTTONS) { Serial.print(keystate->keys[i]);Serial.print(" "); }
			Serial.println("");
		}
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
	update_serial();

    // TIMING
	const uint16_t LOOP_TIME{10000}; // value in microseconds
	elapsedMicros outputTimer; // determines output data rate

	// RENDER
	// Send data out at a controlled rate.
	// Outputs: Serial, Keyboard
	if (outputTimer >= LOOP_TIME) {
		outputTimer -= LOOP_TIME;
		render_output(&ks);
	}
}
