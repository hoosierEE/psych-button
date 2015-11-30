
// CapSwitch.h
// Wraps a Teensy (LC, 3.x) touch-enabled pin with a button interface.

#ifndef CAP_SWITCH_H
#define CAP_SWITCH_H
#include <Arduino.h>

class CapSwitch {
public:
CapSwitch(uint8_t _pin) : pin(_pin),THRESH(200){}

    void update(void)
    {
        val = touchRead(pin);
        buf[++idx%BUFLEN] = val; // update ring buffer
        if (accept) {
            double avg = get_avg();
            if (val - avg > THRESH) { bstate = false; } // cap. increasing
            if (avg - val > THRESH) { bstate = true; } // cap. decreasing
            accept = false;
            hilo = prev_state && !bstate;
            lohi = bstate && !prev_state;
        }
        prev_state = bstate;
    }

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
        return sum/(double)BUFLEN;
    }

    uint8_t pin;
    const int THRESH;
    static const uint8_t BUFLEN{7};
    int buf[BUFLEN]{0};
    int val,idx;
    bool bstate,prev_state,hilo,lohi;
    bool accept{true};
};

#endif // CAP_SWITCH_H
