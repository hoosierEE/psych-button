Psych-Button
============
USB response boxes with real-time accuracy.

In many psychological experiments, researchers want to record subject responses to audio/video stimuli.  In many cases precise and accurate timing is required, and the operating system's time functions are inadequate.  The code in this repository showcases how to make a USB device capable of accurate timing.

The device has a single USB cable but shows up as *two* separate USB devices: a **USB Keyboard** and a **USB Serial** interface.  The keyboard is essentially the same as a standard USB keyboard with the exception that instead of 108 or more keys, it only has 4.  The Serial interface is where things start to get interesting.  This interface provides button press data but *also* precise timing data.

Recording data from the serial interface lets you calculate both the round-trip delay and the offset for the device+PC combo.

### Timing Measurements
Round-trip delay `d` and offset `t` are calculated as per the [SNTP protocol, section 5](https://tools.ietf.org/html/rfc4330#section-5):

    Timestamp Name          ID   When Generated
    ------------------------------------------------------------
    Originate Timestamp     T1   time request sent by client
    Receive Timestamp       T2   time request received by server
    Transmit Timestamp      T3   time reply sent by server
    Destination Timestamp   T4   time reply received by client

    The roundtrip delay d and system clock offset t are defined as:

    d = (T4 - T1) - (T3 - T2)     t = ((T2 - T1) + (T3 - T4)) / 2.

Timing measurement from `psych-button` uses a modified version of SNTP, where `psych-button` responds to a timing request with a single number that corresponds to T3-T2.  In this scheme, T2 is always 0.  T3 is measured in integer microseconds.  Offset is not used, because `psych-button` doesn't keep a wall clock time.  The only figure we're interested in is the round-trip delay.

This procedure is done at least once, usually before an experiment, but can be done at any point in time except during data collection.

    PC (note system time T1), send character: 'T'
    psych-button: T3
    PC (note time of reply T4, calculate d given T3, T2=0)

Python Example:

    import time
    T1 = time.time() # floating point UNIX timestamp e.g. 1444248042.147216
    # todo: send query, receive response, parse T3 from response
    T4 = time.time()
    d = (1000000 * (T4 - T1)) - T3          # d is in units of microseconds

### Button Encoding
Since we have 4 buttons, we can represent "pressed" with a 1 and "not pressed" with a 0, and encode all 4 button values in a single byte:

| State                   | button 3 | button 2 | button 1 | button 0 | Serial representation | Keyboard representation |
|-------------------------|----------|----------|----------|----------|:---------------------:|:-----------------------:|
| no buttons pressed      | 0        | 0        | 0        | 0        |           0           |                         |
| button 0 pressed        | 0        | 0        | 0        | 1        |           1           |      <kbd>w</kbd>       |
|                         | 0        | 0        | 1        | 0        |           2           |      <kbd>a</kbd>       |
|                         | 0        | 0        | 1        | 1        |           3           |                         |
|                         | 0        | 1        | 0        | 0        |           4           |      <kbd>s</kbd>       |
| button 2 pressed        | 0        | 1        | 0        | 1        |           5           |                         |
|                         | 0        | 1        | 1        | 0        |           6           |                         |
|                         | 0        | 1        | 1        | 1        |           7           |                         |
|                         | 1        | 0        | 0        | 0        |           8           |      <kbd>d</kbd>       |
|                         | 1        | 0        | 0        | 1        |           9           |                         |
| buttons 3 and 1 pressed | 1        | 0        | 1        | 0        |           a           |                         |
|                         | 1        | 0        | 1        | 1        |           b           |                         |
|                         | 1        | 1        | 0        | 0        |           c           |                         |
|                         | 1        | 1        | 0        | 1        |           d           |                         |
|                         | 1        | 1        | 1        | 0        |           e           |                         |
| all buttons pressed     | 1        | 1        | 1        | 1        |           f           |                         |

Serial interface updates whenever there is a change in the state of any button (either pressed or released).

The Keyboard interface sends a `keydown` event followed by a `keyup` event when a button is first pressed.

Usage (Keyboard)
----------------
To use a response box as a USB keyboard, plug and play.  By default, the 4 buttons correspond to <kbd>w</kbd>, <kbd>a</kbd>, <kbd>s</kbd>, and <kbd>d</kbd> keyboard events.  Open a text editor and use the response box to "type" letters.

| button | key          |
|--------|:------------:|
| 0      | <kbd>w</kbd> |
| 1      | <kbd>a</kbd> |
| 2      | <kbd>s</kbd> |
| 3      | <kbd>d</kbd> |

Usage (Serial)
--------------
To use the serial interface, open a serial terminal on your computer using a program such as [pyserial](https://github.com/pyserial/pyserial).  Use the following settings:

* 115200 baud
* 8 data bits
* no parity
* 1 stop bit

Pressing and holding the `b0` button (and no others) will result in one line of output:

    1

Meanwhile, if you press the `b3` button, while continuing to hold `b0`, the next output will be:

    9

Finally, releasing the `b3` button, then releasing the `b0` button, will produce this output:

    8
    0

Notes
-----
Source code built with [platformio](http://platformio.org/#!/), a cross-platform command-line based build tool and dependency manager.

Related Reading
---------------
* [Synchronization Explained](http://www.ni.com/white-paper/11369/en/)
* [Precision Time Protocol](https://en.wikipedia.org/wiki/Precision_Time_Protocol)
