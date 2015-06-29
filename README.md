Psych-Button
============
USB response box provides real-time accuracy and requries no custom drivers.

This repository contains code, schematics, and plans for a USB device which allows for taking precise timing measurements.  It is intended for use in psychological experiments with human subjects.  One common scenario is to play a slideshow for the subject and ask them to record their responses to the visual stimuli using the buttons.  Synchronizing stimuli with the subject's responses provides useful data for researchers.

Hex Encoded Buttons
-------------------
This device emits ASCII text via USB Serial emulation.  Use it like a command-line process.  Data is in this format:

    buttons time

where `buttons` is the result of a binary-to-hexadecimal conversion of the button states.  

> The buttons are ordered like this:
> `s3 s2 s1 s0`

For example, four buttons, all "on", would look like this in binary:

    1 1 1 1

In hex, the same four buttons, still "on", would look like this:

    F

Likewise, only the `s3` button on, with the rest off, would look like this in binary:

    1 0 0 0

...and in hex:

    8

Serial USB transmissions (of the button states) are therefore 4x faster.


Buttons in Action
-----------------
Rather than emit data continuously, it only sends data when it detects a change in the state of the buttons.  This means that pressing and holding the `b0` button (and no others) will result in this output:

    1 time

> In binary, that would look like `0 0 0 1 time`

Meanwhile, if you press the `b3` button, while continuing to hold `b0`, the next output will be:

    9 time

> In binary, that would look like `1 0 0 1 time`

Finally, releasing the `b3` button, then releasing the `b0` button, will produce this output:

    8 time
    0 time


That Pesky Time Value
---------------------
In the above examples, `time` represents the value of microseconds in the microcontroller's clock.  The value is an unsigned 32-bit number and represents microseconds.  Values are in the range 0 to 4294967295, inclusive.

notes
-----
Source code built with [platformio](http://platformio.org/#!/), a cross-platform command-line based build tool and dependency manager.
