
// CapSwitch.h
// Easy-to-use button-like wrapper for a Teensy (LC, 3.x) touch-enabled pin.

/* Usage:
   0. Instantiate this class with a pin number.  This pin must be touch-enabled.
   1. Call update() at top of loop() function, as fast as possible.
   2. Call pressed() or released() when you want to read the most recent state.

   Note: if you want to know the value of BOTH pressed() AND released(), call them
   together, without a call to update() between them, as using the "consumer" methods
   (pressed() and released()) allows update() to register new state changes.
*/


#ifndef CAP_SWITCH_H
#define CAP_SWITCH_H
//#include <Arduino.h>

class CapSwitch {
 public:
    // constructor requires touch-enabled pin number
 CapSwitch(uint8_t _pin) : pin(_pin),THRESH(200){}

    void update(void)
    {
        int val = touchRead(pin);
        static int idx{0};
        buf[++idx%BUFLEN] = val; // update ring buffer
        if (accept) { // empty queue
            accept = false;
            double avg = get_avg();
            if (val - avg > THRESH) { bstate = false; } // cap. increasing
            if (avg - val > THRESH) { bstate = true; } // cap. decreasing
            hilo = prev_state && !bstate;
            lohi = bstate && !prev_state;
            prev_state = bstate;
        }
    }

    // int thresh(void) { return THRESH; }
    // void set_thresh(int new_thresh)
    // {
    //     THRESH = new_thresh;
    // }

    bool pressed(void) { return reset_edge(hilo); }
    bool released(void) { return reset_edge(lohi); }

 private:
    bool reset_edge(bool& edge)
    {
        bool t = edge;
        edge = false;
        accept = true;
        return t;
    }

    double get_avg(void)
    {
        double sum{0};
        for (uint8_t i=0;i<BUFLEN;++i) { sum+=buf[i]; }
        return sum/BUFLEN;
    }

    uint8_t pin;
    const int THRESH;
    static const uint8_t BUFLEN{7};
    int buf[BUFLEN]{0};
    bool bstate,prev_state,hilo,lohi;
    bool accept{true};
};

#endif // CAP_SWITCH_H
