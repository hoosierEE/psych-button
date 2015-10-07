Psych-Button
============
USB response boxes with real-time accuracy.

In many psychological experiments, researchers want to record subject responses to audio/video stimuli.  In many cases precise and accurate timing is required, and the operating system's time functions are inadequate.  The code in this repository showcases how to make a USB device capable of accurate timing.

The device has a single USB cable but shows up as *two* separate USB devices: a **USB Keyboard** and a **USB Serial** interface.  The keyboard is essentially the same as a standard USB keyboard with the exception that instead of 108 or more keys, it only has 4.  The Serial interface is where things start to get interesting.  This interface provides button press data but *also* precise timing data.

Recording data from the serial interface lets you calculate both the round-trip delay and the offset for the device+PC combo.

Usage (Keyboard)
----------------
To use a response box as a USB keyboard, plug and play.  By default, the 4 buttons correspond to 'w', 'a','s', and 'd' keycodes.  Open a text editor and use the response box to "type" letters.

button | key
---|---
0 | 'w'
1 | 'a'
2 | 's'
3 | 'd'

Usage (Serial)
--------------
To use the serial interface, open a serial terminal using some program (e.g. [pyserial](https://github.com/pyserial/pyserial)).  Use the following settings:

* 115200 baud
* 8 data bits
* no parity
* 1 stop bit

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

This procedure is done at least once, usually before an experiment, but can be done at any point in time except during data collection.  The PC sends `psych-button` a signal to begin the timing measurement:

    PC: 'T'
    PC [waits for psych-button to respond]
    psych-button [initiate timing algorithm by sending own timestamp]: t1
    PC [T2 = time of t1's arrival]
    PC [T3 = time of reply]: 'K'
    psych-button: t4

where `t1` and `t4` are integer microsecond values (i.e. 1300000 == 1.3 seconds).  `T2` and `T3` depend on the PC's internal representation of system time.  For example, in Python:

    import time
    now = time.time() # floating point UNIX timestamp e.g. 1444248042.147216

If using Python's `time.time()` function, convert `t1` and `t4` to seconds before doing arithmetic.

The PC can now calculate `d` and `t` from `t1 T2 T3 t4` and store these values for later use.

### Begin Signal
To begin data collection, send `psych-button` a begin signal:

    PC: 'B'
    psych-button: ti

where `ti` is the elapsed time since `t4`.  Note that this value will overflow after about 78 minutes, which is why running the timing measurement should be done prior!

Hereafter, `psych-button` will transmit button values as it detects changes to the state of the 4 buttons.

### Button Encoding
Since we have 4 buttons, we can represent "pressed" with a 1 and "not pressed" with a 0, and encode all 4 button values in a single byte:

button 3 | button 2 | button 1 | button 0 | byte representation
---|---|---|---|---
0 | 0 | 0 | 0 | 0
0 | 0 | 0 | 1 | 1
0 | 0 | 1 | 0 | 2
0 | 0 | 1 | 1 | 3
0 | 1 | 0 | 0 | 4
0 | 1 | 0 | 1 | 5
0 | 1 | 1 | 0 | 6
0 | 1 | 1 | 1 | 7
1 | 0 | 0 | 0 | 8
1 | 0 | 0 | 1 | 9
1 | 0 | 1 | 0 | a
1 | 0 | 1 | 1 | b
1 | 1 | 0 | 0 | c
1 | 1 | 0 | 1 | d
1 | 1 | 1 | 0 | e
1 | 1 | 1 | 1 | f

The state of all 4 buttons is sent during each update as a single hexadecimal character.

Rather than emit data continuously, it only sends data when it detects a change in the state of the buttons.  This means that pressing and holding the `b0` button (and no others) will result in one line of output:

    1

Meanwhile, if you press the `b3` button, while continuing to hold `b0`, the next output will be:

    9

Finally, releasing the `b3` button, then releasing the `b0` button, will produce this output:

    8
    0

notes
-----
Source code built with [platformio](http://platformio.org/#!/), a cross-platform command-line based build tool and dependency manager.

Related Reading
---------------
* [Synchronization Explained](http://www.ni.com/white-paper/11369/en/)
* [Precision Time Protocol](https://en.wikipedia.org/wiki/Precision_Time_Protocol)
