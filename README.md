Psych-Button
============
USB response boxes with real-time accuracy.

This repository contains code, schematics, and plans for a USB device which allows for taking precise timing measurements.  It is intended for use in psychological experiments with human subjects.  A typical use case is to play a slideshow for the subject and ask them to record their responses to the visual stimuli using the buttons.  Researchers commonly want to temporally synchronize the slideshow with the buttons.

This device has a single USB cable but enumerates as *two* separate USB devices: a USB Keyboard and a USB Serial interface.  The keyboard may be used for interacting directly with a program (e.g. a slideshow) and the serial interface may be used to obtain precise timing for each button press.

You can use *one or both* of these USB channels, but be aware that timing information is *only* available via serial.

Usage (Keyboard)
----------------
To use a response box as a USB keyboard, plug and play.  By default, the 4 buttons correspond to 'a', 'b','c', and 'd' keycodes.  Open a text editor and use the response box to "type" letters.

Usage (Serial)
--------------
To use the serial interface, open a serial terminal using some program (e.g. [pyserial](https://github.com/pyserial/pyserial)).  Use the following settings:

* 115200 baud
* 8 data bits
* no parity
* 1 stop bit

The serial interface provides timing information by measuring the latency between successive communication intervals.  This requires bidirectional communication with the serial program.  The example program responds to all response box messages with the system time to the nearest microsecond:

    (psych-button): some data
    (PC): 123455.000
    (psych-button): some more data
    (PC): 123456.588

Above, `psych-button` sent some serial data to the host PC.  The PC responded with the system time.  Some time later, `psych-button` sent additional data.  The PC responded with the system time.  The second time was 1.588 seconds after the first.

Key to this approach is that `psych-button` compares the elapsed time given by the PC with the elapsed time as measured by its own internal clock.  Even though the PC reports 1.588 seconds elapsed, `psych-button` may show that only 1.570 seconds elapsed.  Because the mircocontroller is a single process running on hardware, its internal timing is more accurate in measuring real-world events.  The PC doesn't register an event until the USB interrupt handler runs.  Many system events influence when this happens.  From the perspective of a user-space application, USB latency is non-deterministic.

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
In the above examples, `time` represents the value of microseconds in the microcontroller's clock since it received the begin signal.  Values are in the range 0 to 4294967295, inclusive.  This value is transmitted in decimal.

notes
-----
Source code built with [platformio](http://platformio.org/#!/), a cross-platform command-line based build tool and dependency manager.
