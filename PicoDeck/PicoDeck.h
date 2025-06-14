/*!
 * @file PicoDeck.h
 * @brief PicoDeck main control program.
 *
 * @copyright That One Seong, https://github.com/SeongGino, 2025
 * @copyright GNU Lesser General Public License
 *
 * @author [That One Seong](SeongsSeongs@gmail.com)
 * @date 2025
 */

#pragma once

#include "PicoDeckDefines.h"
#include "PicoDeckCommon.h"
#include "PicoDeckDisplay.h"
#include "PicoDeckPrefs.h"

#include <Arduino.h>
#include <Wire.h>
#include <LightgunButtons.h>
#include <Adafruit_NeoPixel.h>
#include <TinyUSB_Devices.h>
// include TinyUSB or HID depending on USB stack option
#if defined(USE_TINYUSB)
#include <Adafruit_TinyUSB.h>
#elif defined(CFG_TUSB_MCU)
#error Incompatible USB stack. Use Adafruit TinyUSB.
#endif

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

// Button descriptor
// The order of the buttons is the order of the button bitmask
// must match ButtonIndex_e order, and the named bitmask values for each button
// see LightgunButtons::Desc_t, format is: 
// {pin, report code 1 (no modifiers), report code 2 (+CTRL), report code 3 (+SHIFT), debounce time, debounce mask}
LightgunButtons::Desc_t LightgunButtons::ButtonDesc[] = {
    {2,   KEY_F13,   KEY_F13,   KEY_F13},    {3,   KEY_F14,   KEY_F14,   KEY_F14},    {4,   KEY_F15,   KEY_F15,   KEY_F15},    {5,   KEY_F16,   KEY_F16,   KEY_F16},
    {6,   KEY_F17,   KEY_F17,   KEY_F17},    {7,   KEY_F18,   KEY_F18,   KEY_F18},    {8,   KEY_F19,   KEY_F19,   KEY_F19},    {9,   KEY_F20,   KEY_F20,   KEY_F20},
    {10,  KEY_F21,   KEY_F21,   KEY_F21},    {11,  KEY_F22,   KEY_F22,   KEY_F22},    {12,  KEY_F23,   KEY_F23,   KEY_F23},    {13,  KEY_F24,   KEY_F24,   KEY_F24},
    {14,  LGB_PREV,  LGB_PREV,  LGB_PREV},   {15,  LGB_NEXT,  LGB_NEXT,  LGB_NEXT}
};

// button count constant
constexpr unsigned int ButtonCount = sizeof(LightgunButtons::ButtonDesc) / sizeof(LightgunButtons::ButtonDesc[0]);

// button runtime data arrays
LightgunButtonsStatic<ButtonCount> lgbData;

LightgunButtons buttons(lgbData, ButtonCount);