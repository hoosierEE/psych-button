
// CapSwitch.h
// Wraps a Teensy (LC, 3.x) touch-enabled pin with a button interface.

#ifndef CAP_SWITCH_H
#define CAP_SWITCH_H
#include <Arduino.h>

class CapSwitch {
 public:
 CapSwitch(uint8_t _pin) : pin(_pin),THRESH(512){}

    void update(void)
    {
        // Produce new transition values IFF previous value has been consumed.
        val = touchRead(pin);
        buf[++idx%BUFLEN] = val; // update ring buffer
        double avg = get_avg(); // get transitions
        // if the capacitance increased AND the state was previously low,
        // change state to HIGH (and vice versa)
        if ((val - avg > THRESH) && !bstate) bstate = true; // TODO: verify
        if ((avg - val > THRESH) && bstate) bstate = false;
    }

    bool pressed(void) { return bstate == true; }
    bool released(void) { return bstate != true; }

 private:
    // average of the buffer
    double get_avg(void)
    {
        double sum{0};
        for (uint8_t i=0;i<BUFLEN;++i) { sum+=buf[i]; }
        return sum/double(BUFLEN);
    }
    uint8_t pin;
    const int THRESH;
    static const uint8_t BUFLEN{7};
    int buf[BUFLEN]{0};
    int val{0};
    int idx{0};
    bool bstate{false};
    bool ready{false};
};

#endif // CAP_SWITCH_H
