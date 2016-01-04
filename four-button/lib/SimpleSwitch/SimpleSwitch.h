// SimpleSwitch.h
// debounce for a polled switch
// Copyright (c) 2014-2015 Trustees of Indiana University
// Author: Alex Shroyer
// Assumes a normally open momentary switch with less than 10ms of bounce.
// Connect to a digital pin; uses internal pull-up resistor.

#ifndef __SIMPLESWITCH_H__
#define __SIMPLESWITCH_H__
#include <Arduino.h>

class SimpleSwitch
{
public:
    // Default constructor. Takes a digital pin.
    SimpleSwitch(uint8_t _pin) : pin(_pin)
    {
        // pin = _pin;
        pinMode(pin,INPUT_PULLUP);
        prev_state = digitalRead(pin);
    }
    ~SimpleSwitch() {}

    // update internal state
    void update(uint32_t now = micros())
    {
        // Update after DEBOUNCE_INTERVAL or more microseconds.
        if (now - prev_time >= DEBOUNCE_INTERVAL) {
            prev_time += DEBOUNCE_INTERVAL;
            current_state = digitalRead(pin);
            // Okay to update the value returned by "pressed()".
            if (accept_next) {
                accept_next = false;
                hilo = prev_state && !current_state; // falling edge reset in pressed()
                lohi = !prev_state && current_state; // rising edge reset in released()
            }

            // Remember switch state.
            prev_state = current_state;
        }
    }

    // Return true once, then always false until next valid transition.  Sometimes called
    // an "immediate" debounce, because it responds to the first transition and ignores further
    // transitions for a set period.
    bool pressed() { return reset_edge(hilo); }
    bool released() { return reset_edge(lohi); }

    bool getState() { return current_state; } // subject to bounce

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
    bool current_state,prev_state,hilo,lohi;
    uint32_t current_time,prev_time;
    const uint32_t DEBOUNCE_INTERVAL{10000}; // 10ms
};

#endif // __SIMPLESWITCH_H__
