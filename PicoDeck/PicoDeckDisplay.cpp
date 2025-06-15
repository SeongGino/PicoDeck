/*!
 * @file PicoDeckDisplay.cpp
 * @brief Wrapper interface for Adafruit_SSD1306 to display Macros interface.
 *
 * @copyright That One Seong, 2025
 * @copyright GNU General Public License
 */

#include <Arduino.h>
#include <Wire.h>

#include "PicoDeckDisplay.h"
#include "PicoDeckCommon.h"
#include "PicoDeckDefines.h"

bool DeckDisplay::Begin(const int &scl, const int &sda, const Adafruit_MultiDisplay::ScreenType_e &displayType)
{
    // Clear out any currently active display, if any
    Stop();

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

        ScreenModeChange(Screen_Default);
        return true;
    } else return false;
}

void DeckDisplay::Stop()
{
    if(display != nullptr) {
        delete display;
        display = nullptr;
    }
}

void DeckDisplay::TopPanelUpdate(const char *mainText, const char *subText)
{
    if(display != nullptr) {
        // draw text in banner canvas
        topBannerBufMain.fillScreen(BLACK);
        topBannerBufMain.setFont(&Sega7x7);
        topBannerBufMain.setCursor(0, 3+SEGAFONT7_HEIGHT);
        topBannerBufMain.print(mainText);
        topBannerBufSub.fillScreen(BLACK);
        topBannerBufSub.setFont(&Sega7x7);
        topBannerBufSub.setCursor(0, 3+SEGAFONT7_HEIGHT);
        if(subText != nullptr) topBannerBufSub.print(subText);
        else topBannerBufSub.print(mainText);

        // draw header line in display buffer if not there
        if(!display->getPixel(0, 15)) display->drawFastHLine(0, 15, 128, WHITE);
        // copy from banner canvas to display render buffer
        display->drawBitmap(0, 0, topBannerBufMain.getBuffer(), topBannerBufMain.width(), topBannerBufMain.height(), WHITE);

        // reset slide animation state
        topBannX = 0;
        topBannScrolling = false;
        lastScrollTimestamp = millis();
    }
}

void DeckDisplay::ScreenModeChange(const ScreenMode_e &screenMode)
{
    if(display != nullptr) {
        display->fillScreen(BLACK);
        //display->fillRect(0, 16, 128, 48, BLACK);

        idleTimeStamp = millis();
        screenState = screenMode;

        switch(screenMode) {
          case Screen_Default:
            PageUpdate(DeckCommon::Prefs->curPage);
            break;
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
          default: break;
        }

        display->display();
        screenUpdated = false;
    }
}

void DeckDisplay::IdleOps()
{
    if(display != nullptr) {
        if(millis() - idleTimeStamp > OLED_IDLE_INTERVAL) {
            idleTimeStamp = millis();

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

            if(screenUpdated) {
                display->display();
                screenUpdated = false;
            }
        }
    }
}

void DeckDisplay::TopPanelScroll()
{
    display->fillRect(0, 3, 128, SEGAFONT7_HEIGHT, BLACK);

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
}

void DeckDisplay::ButtonsUpdate(const uint32_t &btnsMap)
{
    for(int i = 0, b = 0, x = 0, y = 0; b < ButtonCount; ++b) {
        if(LightgunButtons::ButtonDesc[b].reportCode < LightgunButtons::LGB_TYPES) continue;

        if(btnsMap & (1 << b)) {
            for(int p = 0; p < sizeof(keyBoxBitmaps[i]); ++p)
                keyBoxBitmaps[i][p] = ~keyBoxBitmaps[i][p];

            memcpy(keyBoxBuf.getBuffer(), keyBoxBitmaps[i], sizeof(keyBoxBitmaps[i]));
            int xOffset = 32*x;
            int yOffset = 16+(16*y);
            display->fillRect(xOffset, yOffset, keyBoxBuf.width(), keyBoxBuf.height(), BLACK);
            display->drawBitmap(xOffset, yOffset, keyBoxBuf.getBuffer(), keyBoxBuf.width(), keyBoxBuf.height(), WHITE);
            screenUpdated = true;
        }

        ++i;
        if(x < 3) ++x;
        else {
            x = 0;
            ++y;
        }
    }
}

void DeckDisplay::PageUpdate(const uint32_t &page)
{
    display->fillScreen(BLACK);

    char pageStr[16];
    sprintf(pageStr, "         Page %d", page+1);
    switch(page) {
    case 0:  TopPanelUpdate(pageStr, "          Next Page ->"); break;
    case 2:  TopPanelUpdate(pageStr, "<-Prev Page"); break;
    default: TopPanelUpdate(pageStr, "   <-Prev | Next->"); break;
    }

    //display->drawFastVLine(31, 16, 48, WHITE);
    //display->drawFastVLine(63, 16, 48, WHITE);
    display->drawFastVLine(95, 16, 48, WHITE);
    //display->drawFastHLine(0, 31, 128, WHITE);
    //display->drawFastHLine(0, 47, 128, WHITE);

    for(int i = 0, b = 0, x = 0, y = 0; b < ButtonCount; ++b) {
        if(LightgunButtons::ButtonDesc[b].reportCode < LightgunButtons::LGB_TYPES) continue;

        memcpy(keyBoxBuf.getBuffer(), keyBoxBitmaps[i], sizeof(keyBoxBitmaps[i]));
        keyBoxBuf.fillScreen(BLACK);
        // TODO: handle custom combos stuff instead of hard-coded page modifiers
        if(page) {
            keyBoxBuf.setCursor(3, SEGAFONT7_HEIGHT);
            switch(page) {
                case 1: keyBoxBuf.println((char)KEY_LEFT_CTRL); break;
                case 2: keyBoxBuf.println((char)KEY_LEFT_SHIFT); break;
                default: break;
            }
            keyBoxBuf.setCursor(7, SEGAFONT7_HEIGHT+1+SEGAFONT7_HEIGHT);
        } else keyBoxBuf.setCursor(4, 4+SEGAFONT7_HEIGHT);
        keyBoxBuf.print(keyStrings[*(&LightgunButtons::ButtonDesc[b].reportCode+page)-0x20]);
        memcpy(keyBoxBitmaps[i], keyBoxBuf.getBuffer(), sizeof(keyBoxBitmaps[i]));
        ++i;

        int xOffset = 32*x;
        int yOffset = 16+(16*y);

        display->fillRect(xOffset, yOffset, keyBoxBuf.width(), keyBoxBuf.height(), BLACK);
        display->drawBitmap(xOffset, yOffset, keyBoxBuf.getBuffer(), keyBoxBuf.width(), keyBoxBuf.height(), WHITE);

        if(x < 3) ++x;
        else {
            x = 0;
            ++y;
        }
    }

    screenUpdated = true;
}