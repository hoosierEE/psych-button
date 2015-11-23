// CapSwitch.h
// Wraps a Teensy (LC, 3.x) touch-enabled pin with a button interface.

#ifndef CAP_SWITCH_H
#define CAP_SWITCH_H

class CapSwitch {
 public:
    CapSwitch(uint8_t _pin) : pin(_pin), BUFLEN(30), THRESH(500), buf{0}, ready(true), hl(false), lh(false), val(0) {}

    void update(void)
    {
        // Okay to update the value returned by "pressed()".
        if (ready) {
            val = touchRead(pin);
            ready = false; // reset by get_edge()

            // transitions
            // hl true when value decreases by THRESH
            // lh true when value increases by THRESH
            avg = calc_avg(); // TODO write this
            // TODO double check math
            hl = abs(avg - val) > THRESH;
            lh = abs(val - avg) > THRESH;
            static int idx{0};
            idx = (1 + idx) % BUFLEN;
            int sum,avg;
            buf[idx] = val;
            for (uint8_t i=0; i<30; ++i) {
                sum += buf[i];
            }

            //hl = previousState && !val; // reset in pressed()
            //lh = !previousState && val; // reset in released()
        }

        // Remember switch state.
        //previousState = val;
    }

    int get_raw(void)
    {
        return val;
    }

    bool pressed(void) {return get_edge(lh);} 
    bool released(void) {return get_edge(hl);}

 private:
    // accept an edge transition by ref and reset it without modifying other edge
    bool get_edge(bool &edge)
    {
        bool t = edge;
        edge = false;
        ready = true;
        return t;
    }

    uint8_t pin;
    const uint8_t BUFLEN;
    const int THRESH;
    int buf[30];
    bool ready,hl,lh;
    int val;
};

#endif // CAP_SWITCH_H
