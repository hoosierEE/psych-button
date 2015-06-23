Psych-Button
============
USB response box provides real-time accuracy and requries no custom drivers.

This repository contains code, schematics, and plans for a USB device which allows for taking precise timing measurements.  It is intended for use in psychological experiments with human subjects.  One common scenario is to play a slideshow for the subject and ask them to record their responses to the visual stimuli using the buttons.  Synchronizing stimuli with the subject's responses provides useful data for researchers.

**No custom driver** is required to use this project.  It enumerates as *both* a USB keyboard and USB Serial device.  Using one (or both!) of these interfaces is up to the experimenter.

Modes
=====
The device acts as *both* a USB keyboard and a USB serial device, *simultaneously*.  It is up to you to use one or both of these interfaces, according to your needs.

Keyboard Mode
-------------
Each button is assigned a key (e.g. a letter or number).  Use it like a standard USB keyboard.  Emits no timing information.

Serial Mode
-----------
Emits ASCII text over serial-USB.  Use it like a command-line process.  Emits microsecond timestamps and button states, whenever a button is pressed.

About
=====
The code here is being built with [platformio](http://platformio.org/#!/), a cross-platform command-line based build tool and dependency manager.  So far platformio looks very promising.
