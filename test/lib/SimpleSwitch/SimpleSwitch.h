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
        SimpleSwitch(uint8_t p)
        {
            pin = p;
            pinMode(pin,INPUT_PULLUP);
            previousState = digitalRead(pin);
            currentState = previousState;
            hiLoTransition = false;
            acceptNextPress = true;
            currentTime = 0;
        }
        ~SimpleSwitch() {}

        void update(uint32_t now = micros())
        {
            currentTime = now;
            // Update after debounceInterval or more microseconds.
            if (currentTime - previousTime >= debounceInterval) {
                previousTime += debounceInterval;
                currentState = digitalRead(pin);

                // Okay to update the value returned by "pressed()".
                if (acceptNextPress) {
                    acceptNextPress = false;
                    hiLoTransition = previousState && !currentState; // reset in pressed()
                    loHiTransition = !previousState && currentState; // reset in released()
                }

                // Remember switch state.
                previousState = currentState;
            }
        }

        inline bool pressed() { return getTransition(hiLoTransition); }
        inline bool released() { return getTransition(loHiTransition); }

        // Return the raw button state, which is subject to bounce.
        bool getState() { return currentState; }

    private:
        // Return true once, then always false until next valid transition.  Sometimes called
        // an "immediate" debounce, because it responds to the first transition and ignores further
        // transitions for a set period.
        //
        // Takes an edge (rising or falling) BY REFERENCE and resets it.
        // Edge remains false until next update().
        inline bool getTransition(bool& edge)
        {
            bool t = edge;
            edge = false;
            acceptNextPress = true;
            return t;
        }

        bool acceptNextPress;
        bool currentState;
        bool previousState;
        bool hiLoTransition;
        bool loHiTransition;
        uint8_t pin;
        uint32_t currentTime;
        uint32_t previousTime;
        const uint32_t debounceInterval{10000}; // 10ms

};

#endif // __SIMPLESWITCH_H__

