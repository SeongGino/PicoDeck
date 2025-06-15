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
};

// Button descriptor
// The order of the buttons is the order of the button bitmask
// must match ButtonIndex_e order, and the named bitmask values for each button
// see LightgunButtons::Desc_t, format is: 
// {pin, report code 1 (no modifiers), report code 2 (+CTRL), report code 3 (+SHIFT), debounce time, debounce mask}
inline LightgunButtons::Desc_t LightgunButtons::ButtonDesc[] = {
    {2,   KEY_F13,   KEY_F13,   KEY_F13},    {3,   KEY_F14,   KEY_F14,   KEY_F14},    {4,   KEY_F15,   KEY_F15,   KEY_F15},    {5,   KEY_F16,   KEY_F16,   KEY_F16},
    {6,   KEY_F17,   KEY_F17,   KEY_F17},    {7,   KEY_F18,   KEY_F18,   KEY_F18},    {8,   KEY_F19,   KEY_F19,   KEY_F19},    {9,   KEY_F20,   KEY_F20,   KEY_F20},
    {10,  KEY_F21,   KEY_F21,   KEY_F21},    {11,  KEY_F22,   KEY_F22,   KEY_F22},    {12,  KEY_F23,   KEY_F23,   KEY_F23},    {13,  KEY_F24,   KEY_F24,   KEY_F24},
    {14,  LGB_PREV,  LGB_PREV,  LGB_PREV},   {15,  LGB_NEXT,  LGB_NEXT,  LGB_NEXT}
};

// button count constant
static constexpr unsigned int ButtonCount = sizeof(LightgunButtons::ButtonDesc) / sizeof(LightgunButtons::ButtonDesc[0]);

// button runtime data arrays
static inline LightgunButtonsStatic<ButtonCount> lgbData;

static inline LightgunButtons buttons(lgbData, ButtonCount);