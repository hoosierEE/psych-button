// CapSwitch.h
// Wraps a Teensy (LC, 3.x) touch-enabled pin with a button interface.

#ifndef CAP_SWITCH_H
#define CAP_SWITCH_H

class CapSwitch {
 public:
  CapSwitch(uint8_t _pin) :
  pin(_pin), BUFLEN(30), THRESH(500), buf{0}, val(0), idx(0), ready(true), hl(false), lh(false) {}

  void update(void)
  {
    // Okay to update the value returned by "pressed()".
    if (ready) {
      ready = false; // reset by get_edge()
      val = touchRead(pin);

      // update ring buffer
      buf[++idx%BUFLEN] = val;

      // get transitions
      double avg = calc_avg(); // TODO write this
      hl = avg - val > THRESH; // value dropped by THRESH
      lh = val - avg > THRESH; // value raised by THRESH

      //hl = previousState && !val; // reset in pressed()
      //lh = !previousState && val; // reset in released()
      // Remember switch state.
      //previousState = val;
    }
  }

  int get_raw(void) { return val; }
  bool pressed(void) { return get_edge(lh); } 
  bool released(void) { return get_edge(hl); }

 private:

  // average of buf
  double calc_avg(void)
  {
    int sum{0};
    for (uint8_t i = 0; i < BUFLEN; ++i)  sum += buf[i]; 
    return double(sum)/double(BUFLEN);
  }

  // return true if the specified transition occurred,
  // resetting it (to be false) in the process
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
  int buf[30],val,idx;
  bool ready,hl,lh;
};

#endif // CAP_SWITCH_H
