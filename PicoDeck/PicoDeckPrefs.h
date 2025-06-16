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

    std::vector<const char*> pageNames = {
        "Avatar Actions",
        "Scenes",
        "System Apps",
    };

    int pagesCount;

    /// @brief    Constructor
    DeckPrefs();

    /// @brief Initialize filesystem
    /// @return An error code from Errors_e
    Errors_e InitFS();

    /// @brief Load Previously Saved Data
    Errors_e Load();

    /// @brief Save Data to Flash
    Errors_e Save();

    // Local copy of current hotkeys page - changed value signals LED change method
    int curPage = 0;
};