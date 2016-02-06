// SimpleSwitch.h
// debounce for a polled switch
// Author: Alex Shroyer
// Assumes a normally open momentary switch with less than 10ms of bounce.

#ifndef __SIMPLESWITCH_H__
#define __SIMPLESWITCH_H__
#include <Arduino.h>

class SimpleSwitch
{
public:
    // Connect to a digital pin; uses internal pull-up resistor.
    SimpleSwitch(uint8_t _pin) : pin(_pin)
    {
        pinMode(pin,INPUT_PULLUP);
        ps = digitalRead(pin);
    }
    ~SimpleSwitch() {}

    // update internal state
    void update(void)
    {
        // Update after DEBOUNCE_INTERVAL or more microseconds.
        if (to >= DEBOUNCE_INTERVAL) {
            to -= DEBOUNCE_INTERVAL
            cs = digitalRead(pin);
            if (accept_next) { // Okay to update the value returned by "pressed()".
                accept_next = false;
                fall = ps && !cs; // falling edge reset in pressed()
                rise = !ps && cs; // rising edge reset in released()
            }
            ps = cs; // Remember switch state.
        }
    }

    // Debounced getters:
    // Return true once, then ignore subsequent changes until after DEBOUNCE_INTERVAL
    // microseconds have elapsed.  This is sometimes called an "immediate" debounce,
    // compared to other methods which require several reads in order to distinguish
    // a valid state change from a bouncing switch.
    bool pressed(void) { return reset_edge(fall); }
    bool released(void) { return reset_edge(rise); }

    // Raw bouncy getter: For daredevils, or if you want to see how bouncy a switch is.
    bool getState(void) { return cs; }

private:

    // Takes an edge (rising or falling) BY REFERENCE and resets it.
    // Edge remains false until next update().
    bool reset_edge(bool& edge)
    {
        bool t = edge;
        edge = false;
        accept_next = true;
        return t;
    }

    uint8_t pin;
    bool accept_next{true};
    bool cs,ps,fall,rise; // current state, previous state, high-to-low, low-to-high
    elapsedMicros to;
    uint32_t prev; // previous time
    const uint32_t DEBOUNCE_INTERVAL{10000}; // 10ms
};

#endif // __SIMPLESWITCH_H__
