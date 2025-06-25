/*!
 * @file PicoDeckPrefs.h
 * @brief PicoDeck preferences loading/saving.
 *
 * @copyright That One Seong, 2025
 * @copyright GNU General Public License
 *
 * @author [That One Seong](SeongsSeongs@gmail.com)
 * @date 2025
 */

#pragma once

#include <cstdint>
#include <unordered_map>
#include <string_view>
#include <FS.h>
#include <LittleFS.h>

#include "blockImages.h"

class DeckPrefs
{
public:
    /// @brief Error codes
    enum Errors_e {
        Error_Success = 0,
        Error_NoStorage = 1,
        Error_Read = 2,
        Error_NoData = 3,
        Error_Write = 4,
        Error_Erase = 5,
        Error_None = 10
    };

    /// @brief Constructor
    DeckPrefs();

    /// @brief Initialize filesystem
    /// @return An error code from Errors_e
    Errors_e InitFS();

    /// @brief Load Previously Saved Data
    Errors_e Load();

    /// @brief Save Data to Flash
    Errors_e Save();

    /// @brief Strings storage struct for pages vector
    typedef struct Pages_s {
        char name[24];
        uint32_t color;
    } Pages_t;

    /// @brief Pages metadata dynamic array
    /// @details Can be less than total available macro pages as defined in LightgunButtons::ButtonDesc
    // TODO: colors could be enums instead (based on HTML color codes?)
    std::vector<Pages_t> pages = {
        {"Avatar Actions", 0x000000FF},
        {"Scenes", 0x0000FF00},
        {"System Apps", 0x00FF0000},
    };

    typedef struct {
        bool isPacked;
        const uint8_t *ptr;
    } KeyBM_t;

    /// @brief Map of available pushkey bitmaps
    /// @details Keys (effectively a filename) should be less than 16 characters
    /// size should be
    static inline std::unordered_map<std::string_view, KeyBM_t> bitmapsDB = {
        {"none",            {false, no_icon}},
        {"rec_start",       {true,  icon_rec_start}},
        {"rec_pause",       {true,  icon_rec_pause}},
        {"mic_toggle",      {true,  icon_mic_toggle}},
        {"em_norm",         {false, em_norm}},
        {"em_angy",         {false, em_angy}},
        {"em_sad",          {false, em_sad}},
        {"em_smug",         {false, em_smug}},
        {"em_happy",        {false, em_happy}},
        {"em_pout",         {false, em_pout}},
        {"em_confuzz",      {false, em_confuzz}},
        {"em_think",        {false, em_think}},
        {"s_logo",          {false, icon_s_logo}},
        {"dead",            {false, icon_dead}},
        {"washed",          {false, icon_washed}},
        {"pos_set",         {false, icon_pos}},
        {"zoom",            {false, icon_zoom}},
        {"scene_blank",     {false, scene_blank}},
        {"scene_brb",       {false, scene_brb}},
        {"scene_gaming",    {false, scene_gaming}},
    };

    /// @brief Local copy of current hotkeys page from LightgunButtons
    /// @details If comparison to LGB's page value returns false, signals page change for LEDs/OLED
    int curPage = 0;

    bool pagesWrapAround = true;

    bool keyPicNullptrToText = true;
};