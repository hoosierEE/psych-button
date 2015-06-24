//
// Example usage of EventTimer
// Blinks two LEDs at different rates.

#include "EventTimer.h"

uint32_t slow_blink_micros{100000}; // 1 second
uint32_t fast_blink_micros{50000}; // 0.5 second
EventTimer slowblink(slow_blink_micros);
EventTimer fastblink(fast_blink_micros);

uint32_t current; // global to hold the time in microseconds

void setup() {
    // call them with the same value so they stay in sync
    current = micros(); 
    slowblink.begin(current);
    fastblink.begin(current);
}

void loop() {
    current = micros();
    slowblink.update(current);
    fastblink.update(current);
    if(slowblink.hasExpired()) {
        // toggle one of the LEDs
    }
    if(fastblink.hasExpired()) {
        // toggle the other LED
    }

    // do other stuff, because we aren't busy-waiting on delay()
}
