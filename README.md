# Seong's PicoDeck
###### *Working title, don't sue pls
An ~~abomination~~ adaptation of [OpenFIRE Firmware](https://github.com/TeamOpenFIRE/OpenFIRE-Firmware) for use as a macro pad/DIY Stream Deck.

Layout is built around a 4x3 matrix with a display and two extra keys below for pagination - [reference model used for build (Macro Keypad by stink2290)](https://www.thingiverse.com/thing:5166594)

Features:
 - Not based on Python
 - Supports RP2040 and RP2350 chipsets
 - Supports SSD1306 and SH1106/07 monochrome OLEDs (effective sizes 0.96" through 1.3"), displaying macro key bindings and customized page titles (if any)
 - Supports several pages of macros with different key combinations (one key + any eight modifier keys independent of the page) and custom lighting
 - Uses OF's debouncing for glitch-free input reading and queued processing, and Keyboard library with backbuffers for tracking stacked/overlapping keypresses.
 - Some basic multithreading, with inputs/NeoPixels handled on Core0 and display handled on Core1
 - Will probably not make you coffee
 - KUREAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

Runs on Earle Philhower's RP2040/235X [arduino-pico](https://github.com/earlephilhower/arduino-pico) core.

Depends on the following libraries:
 - A minified fork of LightgunButtons and TinyUSB_Devices from the OpenFIRE project (included)
 - Adafruit_NeoPixel: For NeoPixels usage
 - Adafruit_SSD1306: For SSD1306 displays*
 - Adafruit_SH110X: For SH1106/07 displays*
   - *Provided fork used to (eventually) add async DMA transmits
 - Adafruit_GFX: Graphics drawing backend for the above two display libs
