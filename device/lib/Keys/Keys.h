// Keys.h
// Multiple buttons, treated as a single object

#ifndef _KEYS_H_
#define _KEYS_H_

#include "../SimpleSwitch/SimpleSwitch.h"

class Keys {
    public:
        Keys(uint8_t ks[])
        {
            keyCount = sizeof ks / sizeof ks[0];
            for (uint8_t i = 0; i < keyCount; ++i) {
                keys[i] = ks[i];
            }
        }
        ~Keys(){}

        void update(void)
        {
            for (uint8_t i = 0; i < keyCount; ++i) {
            }
        }

        bool changed(void)
        {
            // true if any key chg since last update()
            return chg;
        }

        void reset(void)
        {
            // tabula rasa
            chg = false;
        }

    private:
        static const uint8_t MAX_KEYS{255};
        uint8_t keys[MAX_KEYS]; // current value
        uint8_t prevKeys[MAX_KEYS]; // previous value
        uint8_t keyCount; // number of keys to keep
        bool chg; // previous state != current state
};
#endif // _KEYS_H_

