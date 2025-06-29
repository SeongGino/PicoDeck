/*
 * This module simulates the standard Arduino "Mouse.h" and
 * "Keyboard.h" API for use with the TinyUSB HID API. Instead of doing
 *  #include <HID.h>
 *  #include <Mouse.h>
 *  #include <Keyboard.h>
 *  
 *  Simply do
 *  
 *  #include <TinyUSB_Mouse_Keyboard.h>
 *  
 *  and this module will automatically select whether or not to use the
 *  standard Arduino mouse and keyboard API or the TinyUSB API. We had to
 *  combine them into a single library because of the way TinyUSB handles
 *  descriptors.
 *  
 *  For details on Arduino Mouse.h see
 *   https://www.arduino.cc/reference/en/language/functions/usb/mouse/
 *  For details on Arduino Keyboard.h see
 *   https://www.arduino.cc/reference/en/language/functions/usb/keyboard/
 *
 *  NOTE: This code is derived from the standard Arduino Mouse.h, Mouse.cpp,
 *    Keyboard.h, and Keyboard.cpp code. The copyright on that original code
 *    is as follows.
 *   
 *  Copyright (c) 2015, Arduino LLC
 *  Original code (pre-library): Copyright (c) 2011, Peter Barrett
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _TINYUSB_DEVICES_H_
#define _TINYUSB_DEVICES_H_

#include <Arduino.h>
#include <unordered_set>

/*****************************
 *   GLOBAL SECTION
 *****************************/

class TinyUSBDevices_ {
public:
  /// @brief Constructor
  TinyUSBDevices_() {};

  /// @brief Sends and initializes the array of USB devices to the connected host
  void begin(int polRate);

  /// @brief Array of which of the three devices have new data that should be reported.
  bool newReport = false;
};
extern TinyUSBDevices_ TinyUSBDevices;

/******************************
 *    KEYBOARD SECTION
 ******************************/
  //  Keyboard codes
  //  Note these are different in some respects to the TinyUSB codes but 
  //  are compatible with Arduino Keyboard.h API
  
  #define KEY_LEFT_CTRL   0x80
  #define KEY_LEFT_SHIFT    0x81
  #define KEY_LEFT_ALT    0x82
  #define KEY_LEFT_GUI    0x83
  #define KEY_RIGHT_CTRL    0x84
  #define KEY_RIGHT_SHIFT   0x85
  #define KEY_RIGHT_ALT   0x86
  #define KEY_RIGHT_GUI   0x87
  
  #define KEY_UP_ARROW    0xDA
  #define KEY_DOWN_ARROW    0xD9
  #define KEY_LEFT_ARROW    0xD8
  #define KEY_RIGHT_ARROW   0xD7
  #define KEY_BACKSPACE   0xB2
  #define KEY_TAB       0xB3
  #define KEY_RETURN      0xB0
  #define KEY_ESC       0xB1
  #define KEY_INSERT      0xD1
  #define KEY_DELETE      0xD4
  #define KEY_PAGE_UP     0xD3
  #define KEY_PAGE_DOWN   0xD6
  #define KEY_HOME      0xD2
  #define KEY_END       0xD5
  #define KEY_CAPS_LOCK   0xC1
  #define KEY_F1        0xC2
  #define KEY_F2        0xC3
  #define KEY_F3        0xC4
  #define KEY_F4        0xC5
  #define KEY_F5        0xC6
  #define KEY_F6        0xC7
  #define KEY_F7        0xC8
  #define KEY_F8        0xC9
  #define KEY_F9        0xCA
  #define KEY_F10       0xCB
  #define KEY_F11       0xCC
  #define KEY_F12       0xCD
  #define KEY_F13       0xF0
  #define KEY_F14       0xF1
  #define KEY_F15       0xF2
  #define KEY_F16       0xF3
  #define KEY_F17       0xF4
  #define KEY_F18       0xF5
  #define KEY_F19       0xF6
  #define KEY_F20       0xF7
  #define KEY_F21       0xF8
  #define KEY_F22       0xF9
  #define KEY_F23       0xFA
  #define KEY_F24       0xFB
  
  //  Low level key report: up to 6 keys and shift, ctrl etc at once
  typedef struct
  {
    uint8_t modifiers;
    uint8_t reserved;
    uint8_t keys[6];
  } KeyReport;
  
  /*
   * This class contains the exact same methods as the Arduino Keyboard.h class.
   */
  
  class Keyboard_ : public Print
  {
  private:
    KeyReport _keyReport;

    // storage for keys that were already pressed
    std::unordered_multiset<uint8_t> keyBuffer;
    // storage for modifiers that were already pressed
    uint8_t modsBuffer[8];
  public:
    Keyboard_(void);
    void report();
    size_t write(uint8_t k);
    size_t write(const uint8_t *buffer, size_t size);
    bool press(uint8_t k);
    void pressModifiers(uint8_t m);
    bool release(uint8_t k);
    void releaseModifiers(uint8_t m);
    void releaseAll(void);
  };
extern Keyboard_ Keyboard;

#endif // _TINYUSB_DEVICES_H_
