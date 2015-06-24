// EventTimer.h
// Microsecond Event Timer Class

// Authors: Tony Walker, Alex Shroyer
// Copyright (c) 2012-2015 Trustees of Indiana University

// Use Cases:
//   1) do things at regular intervals
//   2) do multiple things at different intervals
//   3) simulate multitasking on an arduino (e.g. blink LEDs at different rates)
//   4) avoid busy-wait blocking delay()

// Notes:
//   1) Uses microseconds to avoid Arduino millis() rounding errors.
//   2) Does NOT use interrupts.
//   3) Timer overflows do not affect update()
//   4) Minimum interval is set to 1 microsecond

// Usage:
//   1) Create an instance of EventTimer in your sketch
//   2) Call begin() with the current time
//   3) Call update() at the top of loop()
//   4) When hasExpired() returns true, do relevant work

#ifndef __EVENT_TIMER_H__
#define __EVENT_TIMER_H__
#include "Arduino.h"

class EventTimer {
    public:
        // main constructor
        // starts with the timer disabled; call begin() to start the timer
        //
        // params:
        //   originalInterval - the timer interval in microseconds
        explicit EventTimer(uint32_t originalInterval)
        {
            interval = originalInterval;
            expired = false;
            running = false;
        }

        // start the timer
        //
        // @param now (optional uint32_t):
        //  If you want to synchronize several timers to each other:
        //
        //      uint32_t currentTime = micros();
        //      myTimer1.begin(currentTime);
        //      myTimer2.begin(currentTime);
        //
        //  Otherwise call without this parameter for the "current time":
        //
        //      myEventTimerInstance.begin();  // implicit `now`
        //
        //  note:
        //      If interval is zero, this function will refuse to start the timer.
        //      Using exceptions would be a much, much beter solution; however, exceptions
        //      on the Arduino (although they can be enabled) are not enabled using the IDE.
        void begin(uint32_t now = micros())
        {
            currentTime = now;
            if (interval == 0) {
                expired = false;
                running = false;
            }
            else {
                last = now;
                next = now + interval;
                expired = false;
                running = true;
            }
        }

        // stop the timer
        void end(void)
        {
            running = false;
            expired = false;
        }

        // update the timer (using the current micros())
        // test if the timer has expired
        // if so, set the value of `expired` to true, so that future calls to `hasExpired()` will work.
        //
        // Optionally pass a value for `now` to avoid getting the time reported by micros().
        void update(uint32_t now = micros())
        {
            currentTime = now;
            // if the timer has not been started via begin(),
            // exit without doing anything
            if (!running)
                return;

            // reset the dirty flag
            expired = false;
            // determine if the timer has overflowed (next <= now < last) or not (last < next <= now)
            if (((last < next) && (next <= now)) || ((next <= now) && (now < last))) {
                last = next;
                next += interval;
                expired = true;
            }
        }

        inline bool hasExpired(void) const { return expired; }
        inline bool isStarted(void) const { return running; }
        inline uint32_t getLastTimeout(void) const {return last;}
        inline uint32_t getNextTimeout(void) const {return next;}
        inline uint32_t getCurrentTime(void) const {return currentTime;}
        inline uint32_t getInterval(void) const {return interval;}

    private:
        uint32_t last; // when the timer expire last or the timer was started
        uint32_t next; // when the timer will expire next
        uint32_t interval;
        uint32_t currentTime;
        bool expired; // has the timer expired?
        bool running;

}; // class EventTimer

#endif // __EVENT_TIMER_H__

