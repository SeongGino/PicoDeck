 /*!
 * @file PicoDeck.h
 * @brief Shared runtime objects and methods used throughout PicoDeck.
 *
 * @copyright That One Seong, 2025
 * @copyright GNU General Public License
 */ 

#pragma once

#include "PicoDeckPrefs.h"
#include <LightgunButtons.h>
#include <TinyUSB_Devices.h>

class DeckCommon {
public:
    // Instance of preferences data (current page)
    static inline DeckPrefs *Prefs;

    static inline int pagesCount;
};

// Button descriptor
// The order of the buttons is the order of the button bitmask
// must match ButtonIndex_e order, and the named bitmask values for each button
// see LightgunButtons::Desc_t, format is: 
// {pin, report code 1 (no modifiers), report code 2 (+CTRL), report code 3 (+SHIFT), debounce time, debounce mask}
inline LightgunButtons::Desc_t LightgunButtons::ButtonDesc[] = {
    // row 1
    {2,  {KEY_F13,                  KEY_F13 | MOD_CTRL << 4,    KEY_F13 | MOD_SHIFT << 4} },
    {3,  {KEY_F14,                  KEY_F14 | MOD_CTRL << 4,    KEY_F14 | MOD_SHIFT << 4} },
    {4,  {KEY_F15,                  KEY_F15 | MOD_CTRL << 4,    KEY_F15 | MOD_SHIFT << 4} },
    {5,  {KEY_F16,                  KEY_F16 | MOD_CTRL << 4,    KEY_F16 | MOD_SHIFT << 4} },
    // row 2
    {6,  {KEY_F17,                  KEY_F17 | MOD_CTRL << 4,    KEY_F17 | MOD_SHIFT << 4} },
    {7,  {KEY_F18,                  KEY_F18 | MOD_CTRL << 4,    KEY_F18 | MOD_SHIFT << 4} },
    {8,  {KEY_F19,                  KEY_F19 | MOD_CTRL << 4,    KEY_F19 | MOD_SHIFT << 4} },
    {9,  {KEY_F20,                  KEY_F20 | MOD_CTRL << 4,    KEY_F20 | MOD_SHIFT << 4} },
    // row 3
    {10, {KEY_F21,                  KEY_F21 | MOD_CTRL << 4,    KEY_F21 | MOD_SHIFT << 4} },
    {11, {KEY_F22,                  KEY_F22 | MOD_CTRL << 4,    KEY_F22 | MOD_SHIFT << 4} },
    {12, {KEY_F23,                  KEY_F23 | MOD_CTRL << 4,    KEY_F23 | MOD_SHIFT << 4} },
    {13, {KEY_F24,                  KEY_F24 | MOD_CTRL << 4,    KEY_F24 | MOD_SHIFT << 4} },
    // page keys
    {14, {LGB_PREV}},
    {15, {LGB_NEXT}}
};

// button count constant
static constexpr unsigned int ButtonCount = sizeof(LightgunButtons::ButtonDesc) / sizeof(LightgunButtons::ButtonDesc[0]);

// button runtime data arrays
static inline LightgunButtonsStatic<ButtonCount> lgbData;

static inline LightgunButtons buttons(lgbData, ButtonCount);