/*!
 * @file PicoDeckDisplay.h
 * @brief Wrapper interface for many OLED display drivers to render Macros interface.
 *
 * @copyright That One Seong, 2025
 * @copyright GNU General Public License
 */

#pragma once

#include <stdint.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_SH110X.h>
#include "fontSega7x7.h"
#include "PicoDeckDefines.h"
#include "PicoDeckCommon.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

class Adafruit_MultiDisplay {
public:
    enum ScreenType_e {
        NO_DISPLAY = -1,
        I2C_SSD1306 = 0,
        I2C_SH1106,
        I2C_SH1107,
        DISPLAY_TYPES_COUNT
    };

    ScreenType_e dispType = NO_DISPLAY;
    Adafruit_SSD1306 *display1306 = nullptr;
    Adafruit_SH1106G *display1106 = nullptr;
    Adafruit_SH1107 *display1107 = nullptr;

    // constructor
    Adafruit_MultiDisplay(TwoWire *twi, const ScreenType_e &displayType)
        : dispType(displayType)
    {
        switch(displayType) {
        case I2C_SSD1306:
            display1306 = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, twi, -1, 1000000);
            break;
        case I2C_SH1106:
            display1106 = new Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, twi, -1, 1000000);
            break;
        case I2C_SH1107:
            display1107 = new Adafruit_SH1107(SCREEN_WIDTH, SCREEN_HEIGHT, twi, -1, 1000000);
            break;
        default: break;
        }
    }

    // destructor: cleanup
    ~Adafruit_MultiDisplay() {
        switch(dispType) {
        case I2C_SSD1306: delete display1306; break;
        case I2C_SH1106: delete display1106; break;
        case I2C_SH1107: delete display1107; break;
        default: break;
        }
    }

    bool begin() {
        switch(dispType) {
            case I2C_SSD1306:
                return display1306->begin(SSD1306_SWITCHCAPVCC, 0x3C);
            case I2C_SH1106:
                return display1106->begin();
            case I2C_SH1107:
                return display1107->begin();
            default: return false;
        }
    }

    void display() {
        #ifdef SERIAL_DEBUG
        unsigned long preDispTS = millis();
        #endif // SERIAL_DEBUG
        switch(dispType) {
            case I2C_SSD1306:
                display1306->display(); break;
            case I2C_SH1106:
                display1106->display(); break;
            case I2C_SH1107:
                display1107->display(); break;
            default: break;
        }
        #ifdef SERIAL_DEBUG
        Serial.printf("Display() took %d ms\n", millis() - preDispTS);
        #endif // SERIAL_DEBUG
    }

    void invertDisplay(const bool &i) {
        switch(dispType) {
            case I2C_SSD1306:
                display1306->invertDisplay(i); break;
            case I2C_SH1106:
                display1106->invertDisplay(i); break;
            case I2C_SH1107:
                display1107->invertDisplay(i); break;
            default: break;
        }
    }

    // only SSD1306 has a predefined dim function (which sets contrast to 0x8F), with no public "set contrast" method
    void dim(const bool &dim) {
        switch(dispType) {
            case I2C_SSD1306:
                display1306->dim(dim); break;
            case I2C_SH1106:
                display1106->setContrast(dim ? 0x2F : 0xFF);
                break;
            case I2C_SH1107:
                display1107->setContrast(dim ? 0x2F : 0x4F);
                break;
            default: break;
        }
    }

    void cp437(const bool &x) {
        switch(dispType) {
            case I2C_SSD1306:
                display1306->cp437(x); break;
            case I2C_SH1106:
                display1106->cp437(x); break;
            case I2C_SH1107:
                display1107->cp437(x); break;
            default: break;
        }
    }

    void drawFastVLine(const int16_t &x, const int16_t &y, const int16_t &h, const uint16_t &color) {
        switch(dispType) {
            case I2C_SSD1306:
                display1306->drawFastVLine(x, y, h, color); break;
            case I2C_SH1106:
                display1106->drawFastVLine(x, y, h, color); break;
            case I2C_SH1107:
                display1107->drawFastVLine(x, y, h, color); break;
            default: break;
        }
    }

    void drawFastHLine(const int16_t &x, const int16_t &y, const int16_t &w, const uint16_t &color) {
        switch(dispType) {
            case I2C_SSD1306:
                display1306->drawFastHLine(x, y, w, color); break;
            case I2C_SH1106:
                display1106->drawFastHLine(x, y, w, color); break;
            case I2C_SH1107:
                display1107->drawFastHLine(x, y, w, color); break;
            default: break;
        }
    }

    void fillRect(const int16_t &x, const int16_t &y, const int16_t &w, const int16_t &h, const uint16_t &color) {
        switch(dispType) {
            case I2C_SSD1306:
                display1306->fillRect(x, y, w, h, color); break;
            case I2C_SH1106:
                display1106->fillRect(x, y, w, h, color); break;
            case I2C_SH1107:
                display1107->fillRect(x, y, w, h, color); break;
            default: break;
        }
    }

    void fillScreen(const uint16_t &color) {
        switch(dispType) {
            case I2C_SSD1306:
                display1306->fillScreen(color); break;
            case I2C_SH1106:
                display1106->fillScreen(color); break;
            case I2C_SH1107:
                display1107->fillScreen(color); break;
            default: break;
        }
    }

    void drawLine(const int16_t &x0, const int16_t &y0, const int16_t &x1, const int16_t &y1, const uint16_t &color) {
        switch(dispType) {
            case I2C_SSD1306:
                display1306->drawLine(x0, y0, x1, y1, color); break;
            case I2C_SH1106:
                display1106->drawLine(x0, y0, x1, y1, color); break;
            case I2C_SH1107:
                display1107->drawLine(x0, y0, x1, y1, color); break;
            default: break;
        }
    }

    void drawRect(const int16_t &x, const int16_t &y, const int16_t &w, const int16_t &h, const uint16_t &color) {
        switch(dispType) {
            case I2C_SSD1306:
                display1306->drawRect(x, y, w, h, color); break;
            case I2C_SH1106:
                display1106->drawRect(x, y, w, h, color); break;
            case I2C_SH1107:
                display1107->drawRect(x, y, w, h, color); break;
            default: break;
        }
    }

    void drawBitmap(const int16_t &x, const int16_t &y, const uint8_t bitmap[], const int16_t &w, const int16_t &h, const uint16_t &color) {
        switch(dispType) {
            case I2C_SSD1306:
                display1306->drawBitmap(x, y, bitmap, w, h, color); break;
            case I2C_SH1106:
                display1106->drawBitmap(x, y, bitmap, w, h, color); break;
            case I2C_SH1107:
                display1107->drawBitmap(x, y, bitmap, w, h, color); break;
            default: break;
        }
    }

    void drawBitmap(const int16_t &x, const int16_t &y, uint8_t *bitmap, const int16_t &w, const int16_t &h, const uint16_t &color) {
        switch(dispType) {
            case I2C_SSD1306:
                display1306->drawBitmap(x, y, bitmap, w, h, color); break;
            case I2C_SH1106:
                display1106->drawBitmap(x, y, bitmap, w, h, color); break;
            case I2C_SH1107:
                display1107->drawBitmap(x, y, bitmap, w, h, color); break;
            default: break;
        }
    }

    void setTextSize(const uint8_t &s) {
        switch(dispType) {
            case I2C_SSD1306:
                display1306->setTextSize(s); break;
            case I2C_SH1106:
                display1106->setTextSize(s); break;
            case I2C_SH1107:
                display1107->setTextSize(s); break;
            default: break;
        }
    }

    void setFont(const GFXfont *f = NULL) {
        switch(dispType) {
            case I2C_SSD1306:
                display1306->setFont(f); break;
            case I2C_SH1106:
                display1106->setFont(f); break;
            case I2C_SH1107:
                display1107->setFont(f); break;
            default: break;
        }
    }

    void setCursor(const int16_t &x, const int16_t &y) {
        switch(dispType) {
            case I2C_SSD1306:
                display1306->setCursor(x, y); break;
            case I2C_SH1106:
                display1106->setCursor(x, y); break;
            case I2C_SH1107:
                display1107->setCursor(x, y); break;
            default: break;
        }
    }

    void setTextColor(const uint16_t &c) {
        switch(dispType) {
            case I2C_SSD1306:
                display1306->setTextColor(c); break;
            case I2C_SH1106:
                display1106->setTextColor(c); break;
            case I2C_SH1107:
                display1107->setTextColor(c); break;
            default: break;
        }
    }

    void setTextColor(const uint16_t &c, const uint16_t &bg) {
        switch(dispType) {
            case I2C_SSD1306:
                display1306->setTextColor(c, bg); break;
            case I2C_SH1106:
                display1106->setTextColor(c, bg); break;
            case I2C_SH1107:
                display1107->setTextColor(c, bg); break;
            default: break;
        }
    }

    void setTextWrap(const bool &w) {
        switch(dispType) {
            case I2C_SSD1306:
                display1306->setTextWrap(w); break;
            case I2C_SH1106:
                display1106->setTextWrap(w); break;
            case I2C_SH1107:
                display1107->setTextWrap(w); break;
            default: break;
        }
    }

    void print(const char *str) {
        switch(dispType) {
            case I2C_SSD1306:
                display1306->print(str); break;
            case I2C_SH1106:
                display1106->print(str); break;
            case I2C_SH1107:
                display1107->print(str); break;
            default: break;
        }
    }

    void println(const char *str) {
        switch(dispType) {
            case I2C_SSD1306:
                display1306->println(str); break;
            case I2C_SH1106:
                display1106->println(str); break;
            case I2C_SH1107:
                display1107->println(str); break;
            default: break;
        }
    }

    int16_t getCursorX() {
        switch(dispType) {
            case I2C_SSD1306:
                return display1306->getCursorX();
            case I2C_SH1106:
                return display1106->getCursorX();
            case I2C_SH1107:
                return display1107->getCursorX();
            default: return 0;
        }
    }

    int16_t getCursorY() {
        switch(dispType) {
            case I2C_SSD1306:
                return display1306->getCursorY();
            case I2C_SH1106:
                return display1106->getCursorY();
            case I2C_SH1107:
                return display1107->getCursorY();
            default: return 0;
        }
    }

    bool getPixel(const int16_t &x, const int16_t &y) {
        switch(dispType) {
            case I2C_SSD1306:
                return display1306->getPixel(x, y);
            case I2C_SH1106:
                return display1106->getPixel(x, y);
            case I2C_SH1107:
                return display1107->getPixel(x, y);
            default: return false;
        }
    }

    void getTextBounds(const char *str, const int16_t &x, const int16_t &y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) {
        switch(dispType) {
            case I2C_SSD1306:
                display1306->getTextBounds(str, x, y, x1, y1, w, h); break;
            case I2C_SH1106:
                display1106->getTextBounds(str, x, y, x1, y1, w, h); break;
            case I2C_SH1107:
                display1107->getTextBounds(str, x, y, x1, y1, w, h); break;
            default: break;
        }
    }
};

class DeckDisplay {
public:
    enum ScreenMode_e {
        Screen_Init = -1,
        Screen_Default = 0,
        Screen_Saving,
        Screen_SaveSuccess,
        Screen_SaveError
    };

    /// @brief Verifies display pins validity to pass to display constructor, then starts up the display
    /// @return success (true) or fail (false)
    bool Begin(const int &scl, const int &sda, const Adafruit_MultiDisplay::ScreenType_e &displayType);

    /// @brief Update top panel with either a string of 22 characters, or a combination of text prefix and a profile name
    /// @details Prefix should only have up to seven characters, to prevent the profile name being cutoff
    void TopPanelUpdate(const char *mainText, const char *subText = nullptr);

    /// @brief Clear screen for different operational states
    void ScreenModeChange(const ScreenMode_e &screenMode);

    /// @brief Perform maintenance operations (WIP)
    /// @details For when values aren't being updated, but still want to change something on the screen
    /// (i.e. small text printouts when health/ammo empty)
    void IdleOps();

    void TopPanelScroll();

    void ButtonsUpdate(const uint32_t &btnsMap);

    void PageUpdate(const uint32_t &page);

    void SaveUpdate(uint32_t save);

    Adafruit_MultiDisplay *display = nullptr;

private:
    ScreenMode_e screenState = Screen_Init;

    // Set true when screen buffer has new contents to push to display
    bool screenUpdated = false;
    bool topBannUpdated = false;

    enum SavingTypes_e {
        SAVE_STARTED = 0,
        SAVE_FAILED,
        SAVE_SUCCESSFUL
    };

    // TODO: should change library to check for ACKs from both 0x3C/0x3D
    //bool altAddr = false;

    // canvas objects for the top banner's main and subtext (scrolling)
    GFXcanvas1 topBannerBufMain = GFXcanvas1(128, 15);
    GFXcanvas1 topBannerBufSub = GFXcanvas1(128, 15);
    uint8_t topBannerBackupBitmap[((128+7) >> 3) * 15];

    // singleton canvas for keybox objects, and all-in-one buffer for all 12 available keys
    GFXcanvas1 keyBoxBuf = GFXcanvas1(31, 16);
    uint8_t keyBoxBitmaps[4*3][((31+7) >> 3) * 16];

    #define OLED_IDLE_INTERVAL 16

    // timestamps for periodic tasks in IdleOps()
    unsigned long idleTimestamp = 0;

    // OLED dimmer (defaults to ~30min)
    bool oledDimmed = false;
    unsigned long timeoutTimestamp = 0;
    #define OLED_TIMEOUT 1800000

    int topBannX;
    bool topBannScrolling = false;
    unsigned long lastScrollTimestamp = 0;
    #define OLED_SCROLL_INTERVAL 5000

    unsigned long saveResultTimestamp = 0;
    #define OLED_SAVING_TIME 2000
    // Set true when save glyph should be visible (either neutral, failed or success)
    bool saving = false;
    DeckPrefs::Errors_e saveResult = DeckPrefs::Error_None;

    //// Graphics
    // leftover openfire bits
    #define CUSTSPLASHBANN_WIDTH 80
    #define CUSTSPLASHBANN_HEIGHT 16
    static constexpr uint8_t customSplashBanner[] = {
        0x07, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfe, 0xef, 0xf1, 0xff, 0x1f, 0xc0, 0x00, 0x00, 0x00, 0x03, 
        0xfe, 0xef, 0xf9, 0xff, 0x3f, 0xe0, 0x00, 0x00, 0x00, 0x03, 0xfe, 0xef, 0xf9, 0xc0, 0x7a, 0xf0, 
        0x00, 0x00, 0x00, 0x03, 0x80, 0xee, 0x1d, 0xc0, 0x72, 0x73, 0xfc, 0x1f, 0x8f, 0xf3, 0x80, 0xee, 
        0x0d, 0xc0, 0xe2, 0x3b, 0xfe, 0x3f, 0xcf, 0xfb, 0xfe, 0xee, 0x1d, 0xff, 0xff, 0xfb, 0x8f, 0x78, 
        0xef, 0xfb, 0xfe, 0xef, 0xf9, 0xff, 0xe2, 0x3b, 0x87, 0x70, 0x6e, 0x7b, 0xfe, 0xef, 0xf9, 0xc0, 
        0x72, 0x73, 0x8f, 0x7f, 0xee, 0x3b, 0x80, 0xef, 0xe1, 0xc0, 0x7a, 0xf3, 0xfe, 0x70, 0x0e, 0x3b, 
        0x80, 0xee, 0xf1, 0xc0, 0x3f, 0xe3, 0xfc, 0x78, 0xee, 0x3b, 0x80, 0xee, 0x79, 0xff, 0x1f, 0xc3, 
        0x80, 0x3f, 0xce, 0x3b, 0x80, 0xee, 0x3d, 0xff, 0x07, 0x03, 0x80, 0x1f, 0x8e, 0x3b, 0x80, 0xee, 
        0x1d, 0xff, 0x00, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    #define CUSTSPLASH_WIDTH  48
    #define CUSTSPLASH_HEIGHT 48
    static constexpr uint8_t customSplash[] = {
        0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 
        0x00, 0x00, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x03, 
        0xff, 0xff, 0xc0, 0x00, 0x00, 0x07, 0xf8, 0x1f, 0xf0, 0x00, 0x00, 0x1f, 0xc0, 0x03, 0xf8, 0x00, 
        0x00, 0x3f, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x7e, 0x07, 0xe0, 0x7e, 0x00, 0x00, 0xfc, 0x1f, 0xf8, 
        0x3f, 0x00, 0x00, 0xf8, 0x7f, 0xfe, 0x1f, 0x00, 0x01, 0xf0, 0xfc, 0x3f, 0x0f, 0x80, 0x01, 0xe1, 
        0xf0, 0x0f, 0x87, 0x80, 0x03, 0xe3, 0xe0, 0x07, 0xc7, 0xc0, 0x03, 0xc3, 0xc0, 0x03, 0x03, 0xc0, 
        0x07, 0xc7, 0x80, 0x00, 0x03, 0xe0, 0x07, 0x87, 0x80, 0x00, 0x01, 0xe0, 0x07, 0x8f, 0x00, 0x00, 
        0x01, 0xe0, 0x0f, 0x8f, 0x00, 0x00, 0x01, 0xf0, 0x0f, 0x0f, 0x00, 0x00, 0x00, 0xf0, 0x0f, 0x00, 
        0x00, 0x00, 0x00, 0xf0, 0xff, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x7b, 0xff, 0xff, 0xff, 
        0xff, 0x0f, 0x7b, 0xff, 0xff, 0xff, 0x0f, 0x0f, 0x7b, 0xc0, 0x00, 0xf0, 0x0f, 0x0f, 0x7b, 0xc0, 
        0x00, 0xf0, 0x0f, 0x80, 0x03, 0xc0, 0x01, 0xf0, 0x07, 0x8f, 0x7b, 0xc0, 0x01, 0xe0, 0x07, 0x8f, 
        0x7b, 0xc0, 0x01, 0xe0, 0x07, 0xcf, 0x7b, 0xff, 0x83, 0xe0, 0x03, 0xcf, 0x7b, 0xff, 0x83, 0xc0, 
        0x03, 0xe0, 0x03, 0xff, 0x87, 0xc0, 0x01, 0xe0, 0x03, 0xc0, 0x07, 0x80, 0x01, 0xf0, 0x03, 0xc0, 
        0x0f, 0x80, 0x00, 0xf8, 0x03, 0xc0, 0x1f, 0x00, 0x00, 0x7c, 0x03, 0xc0, 0x3e, 0x00, 0x00, 0x7e, 
        0x03, 0xc0, 0x7e, 0x00, 0x00, 0x3f, 0x03, 0xc0, 0xfc, 0x00, 0x00, 0x1f, 0xc3, 0xc3, 0xf8, 0x00, 
        0x00, 0x07, 0xfb, 0xdf, 0xe0, 0x00, 0x00, 0x03, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0xff, 0xff, 
        0x00, 0x00, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 
        0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00
    };

    #define SAVEGLYPH_WIDTH  16
    #define SAVEGLYPH_HEIGHT 14
    static constexpr uint8_t saveGlyph[] = {
        0xff, 0xe0, 0x91, 0xf0, 0x91, 0xe8, 0x91, 0xe4, 0x91, 0xe4, 0x9f, 0xe4, 0x80, 0x04, 0x80, 0x04, 
	    0x80, 0x04, 0x9f, 0xe4, 0x90, 0x24, 0x97, 0xa4, 0x90, 0x24, 0xff, 0xfc
    };

    static constexpr uint8_t saveSuccessGlyph[] = {
        0xff, 0xe0, 0x91, 0xf0, 0x91, 0xe8, 0x91, 0xe4, 0x91, 0xe4, 0x9f, 0xe5, 0x80, 0x03, 0x80, 0x86, 
	    0x80, 0x4c, 0x9f, 0xb8, 0x90, 0x14, 0x97, 0xa4, 0x90, 0x24, 0xff, 0xfc
    };

    // should always be added by 0x20
    static inline const char *keyStrings[] = {
        "   ", // 0x20 - space
        " ! ",
        " \" ",
        " # ",
        " $ ",
        " % ",
        " & ",
        " ' ",
        " ( ",
        " ) ",
        " * ",
        " + ",
        " , ",
        " - ",
        " . ",
        " / ",
        "#0 ",
        "#1 ",
        "#2 ",
        "#3 ",
        "#4 ",
        "#5 ",
        "#6 ",
        "#7 ",
        "#8 ",
        "#9 ",
        " : ",
        " ; ",
        " < ",
        " = ",
        " > ",
        " ? ",
        " @ ",
        " A ",
        " B ",
        " C ",
        " D ",
        " E ",
        " F ",
        " G ",
        " H ",
        " I ",
        " J ",
        " K ",
        " L ",
        " M ",
        " N ",
        " O ",
        " P ",
        " Q ",
        " R ",
        " S ",
        " T ",
        " U ",
        " V ",
        " W ",
        " X ",
        " Y ",
        " Z ",
        " [ ",
        " \\ ",
        " ] ",
        " ^ ",
        " _ ",
        " ` ",
        " a ",
        " b ",
        " c ",
        " d ",
        " e ",
        " f ",
        " g ",
        " h ",
        " i ",
        " j ",
        " k ",
        " l ",
        " m ",
        " n ",
        " o ",
        " p ",
        " q ",
        " r ",
        " s ",
        " t ",
        " u ",
        " v ",
        " w ",
        " x ",
        " y ",
        " z ",
        " { ",
        " | ",
        " } ",
        " ~ ",
        "   ", // 0x7F - last ASCII printable
        "CTRL",// 0x80 - left modifiers
        "SHFT",
        "ALT",
        "META",
        "CTRL",// 0x84 - right modifiers
        "SHFT",
        "ALT",
        "META",
        "",    // 0x88 - nothing
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",    // 0x90
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",    // 0xA0
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "ENTR",    // 0xB0 - Return/Enter
        "ESC",
        "RUB",
        "TAB",
        "",        // 0xB4 - nothing
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",        // 0xC0
        "CAPS",    // 0xC1 - caps lock
        "F1",
        "F2",
        "F3",
        "F4",
        "F5",
        "F6",
        "F7",
        "F8",
        "F9",
        "F10",
        "F11",
        "F12",
        "",       // 0xCE
        "",
        "",       // 0xD0
        "INS",    // 0xD1 - insert
        "",
        "PgUp",
        "DEL",
        "END",
        "PgDn",   // 0xD6 - page down
        "Rght",
        "Left",
        "Dwn",
        "Up",     // 0xDA - Up Arrow
        "",
        "",
        "",
        "",
        "",
        "",       // 0xE0
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "F13",    // 0xF0 - F13
        "F14",
        "F15",
        "F16",
        "F17",
        "F18",
        "F19",
        "F20",
        "F21",
        "F22",
        "F23",
        "F24",    // 0xFB - F24
        "",
        "",
        "",
        ""
    };
};
