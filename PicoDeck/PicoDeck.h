/*!
 * @file PicoDeck.h
 * @brief PicoDeck main control program.
 *
 * @copyright That One Seong, 2025
 * @copyright GNU General Public License
 *
 * @author [That One Seong](SeongsSeongs@gmail.com)
 * @date 2025
 */

#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>
// include TinyUSB or HID depending on USB stack option
#if defined(USE_TINYUSB)
#include <Adafruit_TinyUSB.h>
#elif defined(CFG_TUSB_MCU)
#error Incompatible USB stack. Use Adafruit TinyUSB.
#endif

#include "PicoDeckDefines.h"
#include "PicoDeckDisplay.h"

#define POLL_RATE 1
#define SAVE_INTERVAL 1000

/// @brief      Set NeoPixel(s) color
/// @param      int (x3)
///             R, G, and B color values
/// @param      int
///             Pixel number to set/start fill from
/// @param      bool
///             Set true to fill pixel array from pixel
void PixelUpdate(const int &r, const int &g, const int &b, const int &pixel, const bool &fill = false);

enum FifoCmds_e {
    DISP_BTN_PRESS = 0,
    DISP_PAGE_UPDATE = 1 << 24,
    DECK_SAVING = 2 << 24,
    DISP_BTN_RELEASE = 1 << 30,
};

//// System/Devices
// TinyUSB devices interface object that's initialized in MainCoreSetup
TinyUSBDevices_ TUSBDeviceSetup;

DeckDisplay OLED;

// Local (constant) neopixel object (defined in PicoDeckDefines.h, defaults to GPIO 28/RP2040 A2)
Adafruit_NeoPixel neopixel = Adafruit_NeoPixel(6, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

//// Saving
// Flags true if there's data to save.
bool canSave = false;

//// Timers/synchronization
// Timestamp of last time save was checked.
unsigned long lastSaveChecked = 0;

// Timestamp of last USB packet update.
unsigned long lastUSBpoll = 0;

// Marker for received FIFO signal from opposite core
uint32_t fifoData = 0;

//// Buttons
// bit mask for each button, must match ButtonDesc[] order to match the proper button events
enum ButtonMask_e {
    BtnMask_1 = 1 << 0,
    BtnMask_2 = 1 << 1,
    BtnMask_3 = 1 << 2,
    BtnMask_4 = 1 << 3,
    BtnMask_5 = 1 << 4,
    BtnMask_6 = 1 << 5,
    BtnMask_7 = 1 << 6,
    BtnMask_8 = 1 << 7,
    BtnMask_9 = 1 << 8,
    BtnMask_10 = 1 << 9,
    BtnMask_11 = 1 << 10,
    BtnMask_12 = 1 << 11,
    BtnMask_13 = 1 << 12,
    BtnMask_14 = 1 << 13
};

// button combo to enter pause mode
static inline constexpr uint32_t EnterPauseModeBtnMask = BtnMask_4 | BtnMask_8;