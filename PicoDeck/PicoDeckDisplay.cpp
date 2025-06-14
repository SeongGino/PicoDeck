/*!
 * @file PicoDeckDisplay.cpp
 * @brief Wrapper interface for Adafruit_SSD1306 to display Macros interface.
 *
 * @copyright That One Seong, 2025
 * @copyright GNU Lesser General Public License
 */

#include <Arduino.h>
#include <Wire.h>
#include <TinyUSB_Devices.h>

#include "PicoDeckDisplay.h"
#include "PicoDeckPrefs.h"
#include "PicoDeckDefines.h"

bool DeckDisplay::Begin(const int &scl, const int &sda, const Adafruit_MultiDisplay::ScreenType_e &displayType)
{
    // Clear out any currently active display, if any
    Stop();

    // for some reason, this only works from here? only run this on first go
    if(Sega7x7_InvertedBitmaps[0] == 0) for(int i = 0; i < sizeof(Sega7x7_InvertedBitmaps); ++i)
        Sega7x7_InvertedBitmaps[i] = ~Sega7x7_InvertedBitmaps[i];

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

void DeckDisplay::TopPanelUpdate(const char *textPrefix, const char *profText)
{
    if(display != nullptr) {
        display->fillRect(0, 0, 128, 16, BLACK);
        display->drawRect(0, 15, 128, 1, WHITE);
        display->setTextSize(1);
        display->setTextColor(WHITE);
        display->setFont(&Sega7x7);
        display->setCursor(0, 4+SEGAFONT7_HEIGHT);
        display->print(textPrefix);
        if(profText != nullptr)
            display->print(profText);
    }
}

void DeckDisplay::ScreenModeChange(const ScreenMode_e &screenMode)
{
    if(display != nullptr) {
        display->fillRect(0, 16, 128, 48, BLACK);

        idleTimeStamp = millis();
        screenState = screenMode;

        switch(screenMode) {
          case Screen_Default:
            TopPanelUpdate("Testing Custom Font");
            display->setFont();
            display->setCursor(0, 18);
            display->print("Testing Default Font");
            display->setFont(&Sega7x7_Inverted);
            display->setCursor(0, display->getCursorY()+2+SEGAFONT7_HEIGHT);
            display->println("Testing Inverted Font");
            display->print("And another!");
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
    }
}

void DeckDisplay::IdleOps()
{
    if(display != nullptr) {
        switch(screenState) {
        case Screen_Default:

          break;
        case Screen_Saving:
        default:
          break;
        }
    }
}
