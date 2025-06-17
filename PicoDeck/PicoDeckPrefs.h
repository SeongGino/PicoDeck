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

#include <stdint.h>
#include <FS.h>
#include <LittleFS.h>

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

    /// @brief Local copy of current hotkeys page from LightgunButtons
    /// @details If comparison to LGB's page value returns false, signals page change for LEDs/OLED
    int curPage = 0;
};