// Response button firmware for TeensyLC, built with PlatformIO.
//
// Modified SNTP timing protocol:
//      1. Server sends "initiate timing procedure" request to client
//      2. Client sets an internal stopwatch timer to 0 (T1), and replies with
//         the number of microseconds that elapsed since it received the message (T2)
//      3. Server notes when it receives reply (T3)
//      4. Server repeats steps 1-3 often enough to be statistically meaningful

#include <SimpleSwitch.h> // debounces a pin; provides update(), pressed(), released() methods

// MODEL
NUM_SOMETHING
const uint8_t NUM_BUTTONS{8};
char letters[]{"abcdefgh"}; // default keyboard letters
// buttons connect to TeensyLC pins 2,3,4,5 (top row) and 6,7,8,9 (bottom row)
SimpleSwitch buttons[NUM_BUTTONS]
{ SimpleSwitch(2)
, SimpleSwitch(3)
, SimpleSwitch(4)
, SimpleSwitch(5)
, SimpleSwitch(6)
, SimpleSwitch(7)
, SimpleSwitch(8)
, SimpleSwitch(9)
};
struct KeyState {bool keys[NUM_BUTTONS],changed;} ks;

// FUNCTIONS
int bits_to_int(KeyState *state)
{
	int result{0};
	for (uint8_t i{0}; i < NUM_BUTTONS; ++i) { result |= state->keys[i] << i; }
	return result;
}

void update_buttons(void)
{
	// read 4 pins and update internal button states
	for (uint8_t i{0}; i < NUM_BUTTONS; ++i) { buttons[i].update(); }
}

void print_letters(void)
{
	Serial.println(letters);
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
	// customize letter:button mapping
	// power-cycling restores defaults
	char newLetters[NUM_BUTTONS];
	Serial.readBytes(newLetters,NUM_BUTTONS+1);
	for (uint8_t i{0}; i < NUM_BUTTONS; ++i) {
		letters[i] = valid_letter(newLetters[i]) ? newLetters[i] : letters[i];
	}
	Serial.print("replaced letters with ");
	print_letters();
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
		case 'h':
			Serial.println("Options");
			Serial.println(" h - print this help message");
			Serial.println(" T - print elapsed time");
			Serial.print(" L - customize letters: send up to ");
			Serial.print(NUM_BUTTONS);
			Serial.print(" letters, in order to overwrite the existing ones");
			Serial.println(" l - print the current letter set");
		case 'T':
			// time measurement query response
			Serial.println(micros() - timeSerialArrived); // respond with T3 - T2
			break;
		case 'L':
			customize_keys();
			break;
		case 'l':
			print_letters();
			break;
		default:
			break;
		}
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
			// Send keyboard letter with corresponding button press.
			Keyboard.press(letters[i]);
		}
		if (buttons[i].released()) {
			ks.changed = true;
			ks.keys[i] = false;
			// Release keyboard letter with corresponding button release.
			Keyboard.release(letters[i]);
		}

		// Serial output when button state changes (press OR release)
		if (ks.changed) {
			ks.changed = false; // reset
			// send int-encoded key state
			Serial.println(bits_to_int(&ks));
		}
	}
}

// TIMING
const uint16_t LOOP_TIME{10000}; // value in microseconds
elapsedMicros outputTimer; // determines output data rate

// PROGRAM
void setup() {
	// Only transmits at 9600 baud if RS232 hardware present.
	// Otherwise uses (higher) USB rate.
	Serial.begin(9600);
	Keyboard.begin();
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

