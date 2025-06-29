/*!
 * @file PicoDeckPrefs.cpp
 * @brief PicoDeck preferences loading/saving.
 *
 * @copyright That One Seong, 2025
 * @copyright GNU General Public License
 *
 * @author [That One Seong](SeongsSeongs@gmail.com)
 * @date 2025
 */

#include "PicoDeckPrefs.h"

DeckPrefs::DeckPrefs()
{
    if(InitFS() != Error_Success)
        Serial.println("Flash error!");
    else Load();
}

DeckPrefs::Errors_e DeckPrefs::InitFS()
{
    if(LittleFS.begin())
        return Error_Success;
    else return Error_NoData;
}

DeckPrefs::Errors_e DeckPrefs::Load()
{
    File prefsFile = LittleFS.open("/Prefs.conf", "r");
    if(prefsFile) {
        curPage = prefsFile.read();
        prefsFile.close();
        return Error_Success;
    } else return Error_NoData;
}

DeckPrefs::Errors_e DeckPrefs::Save()
{
    File prefsFile = LittleFS.open("/Prefs.conf", "w");
    if(prefsFile) {
        prefsFile.write((uint8_t)curPage);
        prefsFile.close();
        return Error_Success;
    } else return Error_NoData;
}
