 /*!
 * @file PicoDeck.h
 * @brief Shared runtime objects and methods used throughout PicoDeck.
 *
 * @copyright That One Seong, 2025
 * @copyright GNU Lesser General Public License
 */ 

#pragma once

#include "PicoDeckPrefs.h"

class DeckCommon {
public:
    // Instance of preferences data (current page)
    static inline DeckPrefs *Prefs;
};