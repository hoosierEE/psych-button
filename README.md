Psych-Button
============
USB response boxes for real-time accuracy.

This repository contains code, schematics, and plans for a USB device which allows for taking precise timing measurements.  It is intended for use in psychological experiments with human subjects.  A typical use case is to play a slideshow for the subject and ask them to record their responses to the visual stimuli using the buttons.  Temporally synchronizing stimuli with the subject's responses provides useful data for researchers.

This device has a single USB cable but enumerates as *two separate USB devices*: a USB Keyboard and a USB Serial interface.  The keyboard may be used for interacting directly with a program (e.g. a slideshow) and the serial interface may be used to obtain precise timing for each button press.

> You can use one or both of these USB interfaces, but be aware that timing information is *only* available via serial.

Hex Encoding
------------
This device emits ASCII text via USB Serial emulation.  Use it like a command-line process.  Data is in this format:

    buttons time

where `buttons` is the result of a binary-to-hexadecimal conversion of the button states.  Buttons are in big-endian form: `s3 s2 s1 s0`

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
In the above examples, `time` represents the value of microseconds in the microcontroller's clock.  The value is an unsigned 32-bit number and represents microseconds.  Values are in the range 0 to 4294967295, inclusive.  It is also transmitted in hexadecimal.

notes
-----
Source code built with [platformio](http://platformio.org/#!/), a cross-platform command-line based build tool and dependency manager.
