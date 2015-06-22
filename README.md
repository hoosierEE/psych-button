Psych-Button
============
Real-time, driverless USB response box.

This repository contains code, schematics, and plans for a USB device which allows for taking precise timing measurements.  It is intended for use in psychological experiments with human subjects.  One common scenario is to play a slideshow for the subject and ask them to record their responses to the visual stimuli using the buttons.  Synchronizing stimuli with the subject's responses provides useful data for researchers.

**No custom driver** is required to use this project.  It enumerates as *both* a USB keyboard and USB Serial device.  Using one (or both!) of these interfaces is up to the experimenter.

Implementation
--------------
Inside each box is a microcontroller which provides a real-time clock, USB interface, and button-readings.  It stores no data.  The device operates in two modes simultaneously, and users may choose to use one or both modes in their application:

* Keyboard mode - Each button is assigned a key.
* Serial mode - Emits ASCII data containing timestamps and key states

> Note 1: keyboard mode does not provide timing information at all

> Note 2: Serial mode may require downloading generic USB-serial drivers for your operating system
