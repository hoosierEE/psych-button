// CapSwitch.h
// Wraps a Teensy (LC, 3.x) touch-enabled pin with a button interface.

#ifndef CAP_SWITCH_H
#define CAP_SWITCH_H
#include <Arduino.h>

class CapSwitch {
 public:
 CapSwitch(uint8_t _pin) :
    pin(_pin), THRESH(512), buf{0}, val(0), idx(0), ready(true), hl(false), lh(false) {}

    void update(void)
    {
        // Produce new transition values IFF previous value has been consumed.
        if (ready) {
            ready = false; // reset by reset_edge()
            val = touchRead(pin);
            buf[++idx%BUFLEN] = val; // update ring buffer
            double avg = calc_avg(); // get transitions
            hl = avg - val > THRESH; // capacitance decreased
            lh = val - avg > THRESH; // capacitance increased
        }
    }

    int get_raw(void)    { return val; }
    double get_avg(void) { return calc_avg(); }
    bool pressed(void)   { return reset_edge(lh); }
    bool released(void)  { return reset_edge(hl); }

 private:
    // average of the buffer
    double calc_avg(void)
    {
        double sum{0};
        for (uint8_t i=0;i<BUFLEN;++i) { sum+=buf[i]; }
        return sum/double(BUFLEN);
    }

    // return true if the specified transition occurred,
    // setting it to false the process
    bool reset_edge(bool &edge)
    {
        bool t = edge;
        edge = false;
        ready = true;
        return t;
    }

    uint8_t pin;
    static const uint8_t BUFLEN{7};
    const int THRESH;
    int buf[BUFLEN];
    int val,idx;
    bool ready,hl,lh;
};

#endif // CAP_SWITCH_H
