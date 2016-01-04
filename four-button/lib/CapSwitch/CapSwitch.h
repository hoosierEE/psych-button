/*
  CapSwitch.h
  by Alex Shroyer
  This library makes a capacitance-sensing pin behave like a pushbutton.

  Background:
  When an object (such as a finger) approaches a conductive surface or wire, the
  capacitance increases. More precisely, the capacitance is a function of distance:
  Capacitance = 1 / (distance^2)
  This library keeps track of the average capacitance, and signals a button change
  event when the readings increase or decrease sharply with respect to the average.
  There is also a threshold built in to provide a form of hysteresis, so that small
  fluctuations do not cause false triggers.
  There are some parameters users of this library could tweak to change the
  behavior of these "buttons":
  1. There is currently just a single threshold value, which means the sensitivity
  is equal for both approaching and receding objects. Different values could be
  used for asymmetric thresholds.
  2. The size of the circular buffer is a tradeoff between noise immunity and memory
  use.

  Usage:
  1. Instantiate this class with a pin number (e.g. CapSwitch myCapSwitch(23); ).
  This pin must be touch-enabled.
  2. Call myCapSwitch.update() at top of loop() function, as fast as possible.
  3. Call myCapSwitch.pressed() or myCapSwitch.released() when you want to read
  the most recent state.
  Note: if you want to know the value of BOTH pressed() AND released(), call them
  together, without a call to update() between them, as using the "consumer" methods
  (pressed() and released()) allows update() to register new state changes.

  Extensibility:
  The value returned by touchRead() is an analog value, which means in principle one
  could use this same approach to turn any time-varying analog value into a button.
*/

#ifndef CAP_SWITCH_H
#define CAP_SWITCH_H

class CapSwitch
{
public:
    // constructor requires touch-enabled pin number
    CapSwitch(uint8_t _pin) : pin(_pin),THRESH(200){}
    ~CapSwitch(){}

    void update(void)
    {
        // might want to modify update() to take a function pointer to
        // enable other kinds of analog readings, e.g. analogRead(pin)
        int val = touchRead(pin);
        static int idx{0};
        buf[++idx%BUFLEN] = val; // update ring buffer
        if (accept_next) { // queue empty
            accept_next = false;
            double avg = get_avg();
            current_state = (avg-val>THRESH)||!(val-avg>THRESH);
            hilo = prev_state && !current_state; // falling edge reset in pressed()
            lohi = current_state && !prev_state; // rising edge reset in released()
        }
        prev_state = current_state;
    }

    // might want something like these in the future
    // int get_thresh(void) { return THRESH; }
    // void set_thresh(int new_thresh) {THRESH = new_thresh;}
    bool pressed(void) { return reset_edge(hilo); }
    bool released(void) { return reset_edge(lohi); }

private:

    bool reset_edge(bool &edge)
    {
        bool t = edge;
        edge = false;
        accept_next = true;
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
    bool current_state,prev_state,hilo,lohi;
    bool accept_next{true};
};

#endif // CAP_SWITCH_H
