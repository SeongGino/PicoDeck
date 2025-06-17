/*!
 * @file PicoDeckDisplay.cpp
 * @brief Wrapper interface for many OLED display drivers to render Macros interface.
 *
 * @copyright That One Seong, 2025
 * @copyright GNU General Public License
 */

#include <Arduino.h>
#include <Wire.h>

#include "PicoDeckDisplay.h"
#include "PicoDeckDefines.h"

bool DeckDisplay::Begin(const int &scl, const int &sda, const Adafruit_MultiDisplay::ScreenType_e &displayType)
{
    // Clear out any currently active display, if any
    if(display != nullptr) {
        delete display;
        display = nullptr;
        screenState = Screen_Init;
    }

    TwoWire *twi;

    // TODO: for some reason, doing this AFTER saving updated pins settings (even when doing it from defaults and there's no default mappings for peripheral pins)
    // causes the board to hang. Even though this is all correct (and any display objects should get deleted from the above, so don't think it can be a new object thing)...
    if(scl >= 0 && sda >= 0) {
        if(scl & 2 && sda & 2) {
            // I2C1
            if(scl & 1 && !(sda & 1)) {
                // SDA/SCL are indeed on verified correct pins
                twi = &Wire1;
            } else return false;
        } else if(!(scl & 2) && !(sda & 2)) {
            // I2C0
            if(scl & 1 && !(sda & 1)) {
                // SDA/SCL are indeed on verified correct pins
                twi = &Wire;
            } else return false;
        } else return false;
    } else return false;

    twi->setSDA(sda);
    twi->setSCL(scl);

    display = new Adafruit_MultiDisplay(twi, displayType);

    if(display->begin()) {
        // init backbufs
        memset(topBannerBackupBitmap, 0, sizeof(topBannerBackupBitmap));
        memset(keyBoxBitmaps, 0, sizeof(keyBoxBitmaps));
        topBannerBufMain.setTextWrap(false);
        topBannerBufSub.setTextWrap(false);
        keyBoxBuf.setFont(&Sega7x7);
        keyBoxBuf.setTextWrap(false);

        ScreenModeChange(Screen_Default);
        return true;
    } else return false;
}

void DeckDisplay::TopPanelUpdate(const char *mainText, const PanelTextAlign_e &textAlign, const char *subText, const PanelTextAlign_e &subAlign)
{
    // draw text in banner canvas
    topBannerBufMain.fillScreen(BLACK);
    topBannerBufMain.setFont(&Sega7x7);
    switch(textAlign) {
    case Align_Left:
        topBannerBufMain.setCursor(0, 3+SEGAFONT7_HEIGHT);
        break;
    case Align_Center:
    case Align_Right:
    {
        int16_t x, y;
        uint16_t w, h;
        topBannerBufMain.getTextBounds(mainText, 0, SEGAFONT7_HEIGHT, &x, &y, &w, &h);
        switch(textAlign) {
        case Align_Center: topBannerBufMain.setCursor(64-(w >> 1), 3+SEGAFONT7_HEIGHT); break;
        case Align_Right:  topBannerBufMain.setCursor(128-w, 3+SEGAFONT7_HEIGHT);       break;
        default: break;
        }
        break;
    }
    }
    topBannerBufMain.print(mainText);
    
    
    if(subText == nullptr) memcpy(topBannerBufSub.getBuffer(), topBannerBufMain.getBuffer(), ((topBannerBufMain.width()+7) >> 3) * topBannerBufMain.height());
    else {
        topBannerBufSub.fillScreen(BLACK);
        topBannerBufSub.setFont(&Sega7x7);
        switch(subAlign) {
        case Align_Left:
            topBannerBufSub.setCursor(0, 3+SEGAFONT7_HEIGHT);
            break;
        case Align_Center:
        case Align_Right:
        {
            int16_t x, y;
            uint16_t w, h;
            topBannerBufSub.getTextBounds(subText, 0, SEGAFONT7_HEIGHT, &x, &y, &w, &h);
            switch(subAlign) {
            case Align_Center: topBannerBufSub.setCursor(64-(w >> 1), 3+SEGAFONT7_HEIGHT); break;
            case Align_Right:  topBannerBufSub.setCursor(128-w, 3+SEGAFONT7_HEIGHT);       break;
            default: break;
            }
            break;
        }
        }
        topBannerBufSub.print(subText);
    }

    // draw header line in display buffer if not there
    if(!display->getPixel(0, 15)) display->drawFastHLine(0, 15, 128, WHITE);
    // copy from banner canvas to display render buffer
    display->drawBitmap(0, 0, topBannerBufMain.getBuffer(), topBannerBufMain.width(), topBannerBufMain.height(), WHITE);

    // mark that banner's been updated
    topBannUpdated = true;
    // reset slide animation state
    topBannX = 0;
    topBannScrolling = false;
    lastScrollTimestamp = millis();
}

void DeckDisplay::ScreenModeChange(const ScreenMode_e &screenMode)
{
    display->fillScreen(BLACK);

    idleTimestamp = millis();
    if(screenState != screenMode) {
        screenState = screenMode;

        switch(screenMode) {
            case Screen_Default:
                PageUpdate(DeckCommon::Prefs->curPage);
                break;
            /*
            case Screen_Saving:
                TopPanelUpdate("Saving Profiles");
                display->setTextSize(2);
                display->setCursor(16, 18);
                display->print("Saving...");
                break;
            case Screen_SaveSuccess:
                display->setTextSize(2);
                display->setCursor(30, 18);
                display->print("Save");
                display->setCursor(4, 40);
                display->print("successful");
                break;
            case Screen_SaveError:
                display->setTextSize(2);
                display->setCursor(30, 18);
                display->print("Save");
                display->setCursor(22, 40);
                display->print("failed");
                break;
            */
            default: break;
        }

        display->display();
        screenUpdated = false;
        topBannUpdated = false;
        // constitutes a wakeup
        if(oledDimmed) display->dim(false);
        oledDimmed = false;
        timeoutTimestamp = millis();
    }
}

void DeckDisplay::IdleOps()
{
    if(millis() - idleTimestamp > OLED_IDLE_INTERVAL) {
        idleTimestamp = millis();

        switch(screenState) {
        case Screen_Default:
            if(topBannScrolling) TopPanelScroll();
            else if(millis() - lastScrollTimestamp > OLED_SCROLL_INTERVAL) {
                topBannScrolling = true;
                TopPanelScroll();
            }
            break;
        default: break;
        }

        if(saving) {
            if(saveResult == DeckPrefs::Error_None) {
                if(topBannUpdated) {
                    // clear space out for save glyph so it overlays previous contents
                    display->fillRect(128-SAVEGLYPH_WIDTH, 0, SAVEGLYPH_WIDTH, SAVEGLYPH_HEIGHT, BLACK);
                    display->drawBitmap(128-SAVEGLYPH_WIDTH, 0, saveGlyph, SAVEGLYPH_WIDTH, SAVEGLYPH_HEIGHT, WHITE);
                }
            } else {
                if(millis() - saveResultTimestamp > OLED_SAVING_TIME) {
                    saving = false;
                    if(!topBannUpdated) {
                        // clear dangling save glyph since it's finished (and rerender top panel text)
                        display->fillRect(128-SAVEGLYPH_WIDTH, 0, SAVEGLYPH_WIDTH, SAVEGLYPH_HEIGHT, BLACK);
                        display->drawBitmap(0, 0, topBannerBufMain.getBuffer(), topBannerBufMain.width(), topBannerBufMain.height(), WHITE);
                        screenUpdated = true;
                        topBannUpdated = true;
                    }
                } else if(topBannUpdated) {
                    // make sure save glyph is overlayed atop any previously rendered contents
                    display->fillRect(128-SAVEGLYPH_WIDTH, 0, SAVEGLYPH_WIDTH, SAVEGLYPH_HEIGHT, BLACK);
                    switch(saveResult) {
                        case DeckPrefs::Error_Success:
                            display->drawBitmap(128-SAVEGLYPH_WIDTH, 0, saveSuccessGlyph, SAVEGLYPH_WIDTH, SAVEGLYPH_HEIGHT, WHITE);
                            break;
                        default: break;
                    }
                }
            }
        }

        if(screenUpdated) {
            display->display();
            screenUpdated = false;
            topBannUpdated = false;
        }

        if(!oledDimmed && millis() - timeoutTimestamp > OLED_TIMEOUT) {
            display->dim(true);
            oledDimmed = true;
        }
    }
}

void DeckDisplay::TopPanelScroll()
{
    //display->fillRect(0, 3, 128, SEGAFONT7_HEIGHT, BLACK);
    display->fillRect(0, 0, 128, 15, BLACK);

    if(topBannX >= 128) {
        // scrolling has finished
        topBannX = 0;
        topBannScrolling = false;
        lastScrollTimestamp = millis();
        // swap sub and main bitmaps
        memcpy(topBannerBackupBitmap, topBannerBufMain.getBuffer(), sizeof(topBannerBackupBitmap));
        memcpy(topBannerBufMain.getBuffer(), topBannerBufSub.getBuffer(), sizeof(topBannerBackupBitmap));
        memcpy(topBannerBufSub.getBuffer(), topBannerBackupBitmap, sizeof(topBannerBackupBitmap));
    } else display->drawBitmap(topBannX-128, 0, topBannerBufSub.getBuffer(), topBannerBufSub.width(), topBannerBufSub.height(), WHITE);

    display->drawBitmap(topBannX++, 0, topBannerBufMain.getBuffer(), topBannerBufMain.width(), topBannerBufMain.height(), WHITE);

    screenUpdated = true;
    topBannUpdated = true;
}

void DeckDisplay::ButtonsUpdate(const uint32_t &btnsMap, const bool &isReleased)
{
    for(int i = 0, b = 0, x = 0, y = 0; b < (int)ButtonCount; ++b) {
        if(LightgunButtons::ButtonDesc[b].keys.at(0) < LightgunButtons::LGB_PAGEKEYS) continue;

        // invert btn bitmap to match state (if not already matching)
        // check top-leftmost pixel - on = pressed previously, off = wasn't pressed
        if(btnsMap & (1 << b) && ((isReleased && keyBoxBitmaps[i][0]) || (!isReleased && !keyBoxBitmaps[i][0]))) {
            for(int p = 0; p < (int)sizeof(keyBoxBitmaps[i]); ++p)
                keyBoxBitmaps[i][p] = ~keyBoxBitmaps[i][p];

            int xOffset = 32*x;
            int yOffset = 16+(16*y);

            display->fillRect(xOffset, yOffset, keyBoxBuf.width(), keyBoxBuf.height(), BLACK);
            display->drawBitmap(xOffset, yOffset, keyBoxBitmaps[i], keyBoxBuf.width(), keyBoxBuf.height(), WHITE);

            screenUpdated = true;
        }   

        ++i;
        // increment/loop around current row and column for next btn
        if(x < 3) ++x;
        else {
            x = 0;
            ++y;
        }
    }
    // constitutes a wakeup
    if(oledDimmed) display->dim(false);
    oledDimmed = false;
    timeoutTimestamp = millis();
}

void DeckDisplay::PageUpdate(const uint32_t &page)
{
    // reject page num if over amount of pages
    if(page >= (uint)DeckCommon::pagesCount) return;

    display->fillScreen(BLACK);

    char pageStr[40];
    if(DeckCommon::Prefs->pages.size() > page && DeckCommon::Prefs->pages.at(page).name[0] != 0)
         sprintf(pageStr, "Page %d: %s", (int8_t)page+1, DeckCommon::Prefs->pages.at(page).name);
    else sprintf(pageStr, "Page %d", (int8_t)page+1);

    if(page == (uint)DeckCommon::pagesCount-1)
         TopPanelUpdate(pageStr, Align_Center, "<-Prev Page", Align_Left);
    else if(!page)
         TopPanelUpdate(pageStr, Align_Center, "Next Page ->", Align_Right);
    else TopPanelUpdate(pageStr, Align_Center, "<-Prev        Next->", Align_Center);

    //display->drawFastVLine(31, 16, 48, WHITE);
    //display->drawFastVLine(63, 16, 48, WHITE);
    display->drawFastVLine(95, 16, 48, WHITE);
    //display->drawFastHLine(0, 31, 128, WHITE);
    //display->drawFastHLine(0, 47, 128, WHITE);

    for(int i = 0, b = 0, x = 0, y = 0; b < (int)ButtonCount; ++b) {
        if((LightgunButtons::ButtonDesc[b].keys.at(0) & 0xFF) < LightgunButtons::LGB_PAGEKEYS) continue;

        keyBoxBuf.fillScreen(BLACK);

        if(LightgunButtons::ButtonDesc[b].keys.size() > page && LightgunButtons::ButtonDesc[b].keys.at(page)) {
            if(LightgunButtons::ButtonDesc[b].keys.at(page) & 0xFF00) {
                keyBoxBuf.setCursor(3, SEGAFONT7_HEIGHT);
                for(int k = 0; k < 8; ++k)
                    if(LightgunButtons::ButtonDesc[b].keys.at(page) & (0x0100 << k)) keyBoxBuf.write((char)0x80+k);
                
                keyBoxBuf.setCursor(7, SEGAFONT7_HEIGHT+1+SEGAFONT7_HEIGHT);
            } else keyBoxBuf.setCursor(4, 4+SEGAFONT7_HEIGHT);
            keyBoxBuf.print(keyStrings[(LightgunButtons::ButtonDesc[b].keys.at(page) & 0xFF)-0x20]);
        }

        // copy finished canvas to this button's backbuffer
        memcpy(keyBoxBitmaps[i], keyBoxBuf.getBuffer(), sizeof(keyBoxBitmaps[i]));
        ++i;

        int xOffset = 32*x;
        int yOffset = 16+(16*y);

        // render btn to display buffer
        display->fillRect(xOffset, yOffset, keyBoxBuf.width(), keyBoxBuf.height(), BLACK);
        display->drawBitmap(xOffset, yOffset, keyBoxBuf.getBuffer(), keyBoxBuf.width(), keyBoxBuf.height(), WHITE);

        if(x < 3) ++x;
        else {
            x = 0;
            ++y;
        }
    }

    screenUpdated = true;
    // constitutes a wakeup
    if(oledDimmed) display->dim(false);
    oledDimmed = false;
    timeoutTimestamp = millis();
}

void DeckDisplay::SaveUpdate(uint32_t save)
{
    saving = true;
    if(save) {
        saveResult = (DeckPrefs::Errors_e)--save;
        saveResultTimestamp = millis();
    } else saveResult = DeckPrefs::Error_None;
    topBannUpdated = true;
    screenUpdated = true;
}