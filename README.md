Psych-Button
============
USB response box provides real-time accuracy and requries no custom drivers.

This repository contains code, schematics, and plans for a USB device which allows for taking precise timing measurements.  It is intended for use in psychological experiments with human subjects.  One common scenario is to play a slideshow for the subject and ask them to record their responses to the visual stimuli using the buttons.  Synchronizing stimuli with the subject's responses provides useful data for researchers.

About
-----
This device emits ASCII text via USB Serial emulation.  Use it like a command-line process.  Data is in this format:

    (b0 b1 b2 b3 time)

Rather than emit data continuously, it only sends data when it detects a change in the state of the buttons.  This means that pressing and holding the `b0` button (and no others) will result in this output:

    (1 0 0 0 time)

Meanwhile, if you press the `b3` button, while continuing to hold `b0`, the next output will be:

    (1 0 0 1 time)

Finally, releasing the `b3` and then the `b0` buttons will produce this output:

    (1 0 0 0 time)
    (0 0 0 0 time)

Note that in the above examples, the value for `time` will increase with each output.  Since this value is in microseconds, the value will be a large integer, for example `12875259`.

notes
-----
Source code built with [platformio](http://platformio.org/#!/), a cross-platform command-line based build tool and dependency manager.
