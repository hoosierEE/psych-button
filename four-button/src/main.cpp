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

// TIMING
const uint16_t LOOP_TIME{10000}; // value in microseconds
elapsedMicros outputTimer; // determines output data rate

// MODEL
const uint8_t NUM_BUTTONS{4};
char l[]{"wxyz"}; // default keyboard TODO make sure "string" notation works with array accesses
char h[]{"_-"}; // home button down(_) or up(-)
// buttons connect to TeensyLC pins 2,3,4,5
SimpleSwitch buttons[NUM_BUTTONS]{SimpleSwitch(2),SimpleSwitch(3),SimpleSwitch(4),SimpleSwitch(5)};
struct KeyState {bool keys[NUM_BUTTONS],changed;} ks;

// FUNCTIONS
void update_buttons(void){DO(NUM_BUTTONS){buttons[i].update();}} // update buttons with pin readings
bool valid_letter(char c) {return ((c>='0'&&c<='9')||(c>='A'&&c<='Z')||(c>='a'&&c<='z'));} // [09AZaz]=true else false

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

void update_serial(void)
{
	// process incoming commands from the Serial buffer, if any
	// accepts the following 1-letter commands:
	// T (Time)
	// l (list letters)
	// L (change letters)
	if (Serial.available()) {
		uint32_t t2{micros()};
		switch (Serial.read()) {
		case 'T': Serial.println(micros() - t2); break; // respond with T3 - T2
		case 'L': customize_keys(); break;
		case 'l': Serial.println(l); break;
		default: break;
		}
	}
}

void render_output(KeyState *k)
{
	// Send data to Keyboard, Serial, or both.
	DO(NUM_BUTTONS) {
		// update internal key state
		if (buttons[i].pressed()) {
			k->changed = true;
			k->keys[i] = true;
			// Send keyboard letter with corresponding button press.
			Keyboard.press(l[i]);
		}
		if (buttons[i].released()) {
			k->changed = true;
			k->keys[i] = false;
			// Release keyboard letter with corresponding button release.
			Keyboard.release(l[i]);
		}

		// Serial output when button k changes (press OR release)
		if (k->changed) {
			k->changed = false; // reset
			const uint8_t BLEN{9};
			char buf[BLEN]={0}; // holds an array of null terminators
			snprintf(buf,BLEN,"%d %d %d %d\n",k->keys[0],k->keys[1],k->keys[2],k->keys[3]);
			Serial.print(buf);
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

	// RENDER
	// Send data out at a controlled rate.
	// Outputs: Serial, Keyboard
	if (outputTimer >= LOOP_TIME) {
		outputTimer -= LOOP_TIME;
		render_output(&ks);
	}
}
