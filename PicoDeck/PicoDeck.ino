/*!
 * @file PicoDeck.ino
 * @brief PicoDeck - Seong's Deranged af macro device firmware thingy,
 * using mutilated bits and pieces from the OpenFIRE project from https://github.com/TeamOpenFIRE/OpenFIRE-Firmware,
 * which is forked from IR-GUN4ALL v4.2, which is based on Prow's Enhanced Fork from https://github.com/Prow7/ir-light-gun,
 * which in itself is based on the 4IR Beta "Big Code Update" SAMCO project from https://github.com/samuelballantyne/IR-Light-Gun
 * Yes, I am an insane human being.
 * Somehow this is all Cynthia's fault.
 *
 * @copyright That One Seong, 2025
 * @copyright GNU General Public License
 *
 * @author [That One Seong](SeongsSeongs@gmail.com)
 * @date 2025
 */

#include "PicoDeck.h"

void setup()
{
    #ifdef SERIAL_DEBUG
    Serial.begin(9600);
    Serial.setTimeout(0);
    delay(1000);
    #endif // SERIAL_DEBUG

    // Initializes TinyUSB identifier
    // Values are pulled from EEPROM values that were loaded earlier in setup()
    TinyUSBDevice.setManufacturerDescriptor(MANUFACTURER_NAME);
    TinyUSBDevice.setProductDescriptor(DEVICE_NAME);
    TinyUSBDevice.setID(DEVICE_VID, DEVICE_PID);

    // Initializing the USB devices chunk.
    TUSBDeviceSetup.begin(POLL_RATE);
    // wait until device mounted
    while(!USBDevice.mounted()) yield();

    DeckCommon::pagesCount = buttons.Begin();
    #ifndef SERIAL_DEBUG
    buttons.ReportEnable();
    #endif // SERIAL_DEBUG

    DeckCommon::Prefs = new DeckPrefs();
    buttons.page = DeckCommon::Prefs->curPage;

    // get Core1 (display) running
    rp2040.fifo.push(0);

    neopixel.begin();
    
    if(DeckCommon::Prefs->curPage < DeckCommon::pagesCount)
         PixelUpdate(DeckCommon::Prefs->pages.at(DeckCommon::Prefs->curPage).color, 0, true);
    else PixelUpdate(0, 0, 0, 0, true);
}

void setup1()
{
    // wait for signal from Core1, which should be after prefs have loaded
    rp2040.fifo.pop();

    // In case some I2C devices deadlock the program
    // (can happen due to bad pin mappings)
    Wire.setTimeout(100);
    Wire1.setTimeout(100);

    // Unga bunga way of setting defaults
    // Maybe someday this can be made more dynamic, preferably in PicoDeckPrefs (ha)
    memset(OLED.keyPics, 0, sizeof(OLED.keyPics));
    OLED.keyPics[0][0] = &DeckCommon::Prefs->bitmapsDB.at("em_angy");       OLED.keyPics[0][1] = &DeckCommon::Prefs->bitmapsDB.at("scene_blank");
    OLED.keyPics[1][0] = &DeckCommon::Prefs->bitmapsDB.at("em_happy");      OLED.keyPics[1][1] = &DeckCommon::Prefs->bitmapsDB.at("scene_brb");
    OLED.keyPics[2][0] = &DeckCommon::Prefs->bitmapsDB.at("em_smug");       OLED.keyPics[2][1] = &DeckCommon::Prefs->bitmapsDB.at("scene_gaming");
    OLED.keyPics[3][0] = &DeckCommon::Prefs->bitmapsDB.at("s_logo");        OLED.keyPics[3][1] = &DeckCommon::Prefs->bitmapsDB.at("washed");        OLED.keyPics[3][2] = &DeckCommon::Prefs->bitmapsDB.at("rec_start");

    OLED.keyPics[4][0] = &DeckCommon::Prefs->bitmapsDB.at("em_pout");       OLED.keyPics[4][1] = &DeckCommon::Prefs->bitmapsDB.at("none");
    OLED.keyPics[5][0] = &DeckCommon::Prefs->bitmapsDB.at("em_norm");       OLED.keyPics[5][1] = &DeckCommon::Prefs->bitmapsDB.at("none");
    OLED.keyPics[6][0] = &DeckCommon::Prefs->bitmapsDB.at("none");          OLED.keyPics[6][1] = &DeckCommon::Prefs->bitmapsDB.at("none");
    OLED.keyPics[7][0] = &DeckCommon::Prefs->bitmapsDB.at("zoom");          OLED.keyPics[7][1] = &DeckCommon::Prefs->bitmapsDB.at("none");          OLED.keyPics[7][2] = &DeckCommon::Prefs->bitmapsDB.at("mic_toggle");

    OLED.keyPics[8][0] = &DeckCommon::Prefs->bitmapsDB.at("em_think");      OLED.keyPics[8][1] = &DeckCommon::Prefs->bitmapsDB.at("none");
    OLED.keyPics[9][0] = &DeckCommon::Prefs->bitmapsDB.at("em_sad");        OLED.keyPics[9][1] = &DeckCommon::Prefs->bitmapsDB.at("none");
    OLED.keyPics[10][0] = &DeckCommon::Prefs->bitmapsDB.at("em_confuzz");   OLED.keyPics[10][1] = &DeckCommon::Prefs->bitmapsDB.at("none");
    OLED.keyPics[11][0] = &DeckCommon::Prefs->bitmapsDB.at("pos_set");      OLED.keyPics[11][1] = &DeckCommon::Prefs->bitmapsDB.at("none");         OLED.keyPics[11][2] = &DeckCommon::Prefs->bitmapsDB.at("rec_pause");

    if(OLED.Begin(DISP_SCL, DISP_SDA, Adafruit_MultiDisplay::I2C_SSD1306) == false) {
        // Does this ever actually happen...?
        #ifdef SERIAL_DEBUG
        Serial.println("Display init error!");
        #endif // SERIAL_DEBUG
    }
}

void loop() {
    buttons.Poll(0);

    if(buttons.pressed) {
        rp2040.fifo.push(buttons.pressed | DISP_BTN_PRESS);
        #ifdef SERIAL_DEBUG
        for(int i = 0; i < (int)ButtonCount; ++i) if(buttons.pressed & 1 << i) {
            if(LightgunButtons::ButtonDesc[i].keys.size() > buttons.page)
                 Serial.printf("Pressed Button %d (Key: %d)\n", i+1, LightgunButtons::ButtonDesc[i].keys.at(buttons.page) & 0xFF);
            else Serial.printf("Pressed Button %d (No keybind)\n", i+1);
            }
        #endif // SERIAL_DEBUG
    }
    if(buttons.released)
        rp2040.fifo.push(buttons.released | DISP_BTN_RELEASE);

    if(millis() - lastUSBpoll >= POLL_RATE) {
        lastUSBpoll = millis();
        buttons.SendReports();
    }

    if(buttons.page != DeckCommon::Prefs->curPage) {
        DeckCommon::Prefs->curPage = buttons.page;
        rp2040.fifo.push(buttons.page | DISP_PAGE_UPDATE);
        
        #ifdef SERIAL_DEBUG
        Serial.printf("Switched to page %d\n", DeckCommon::Prefs->curPage+1);
        #endif // SERIAL_DEBUG

        if(DeckCommon::Prefs->curPage < DeckCommon::pagesCount)
             PixelUpdate(DeckCommon::Prefs->pages.at(DeckCommon::Prefs->curPage).color, 0, true);
        else PixelUpdate(0, 0, 0, 0, true);
        
        canSave = true;
        lastSaveChecked = millis();
    }

    if(canSave && millis() - lastSaveChecked >= SAVE_INTERVAL) {
        canSave = false;
        rp2040.fifo.push(DECK_SAVING);
        DeckPrefs::Errors_e saveResult = DeckCommon::Prefs->Save();
        rp2040.fifo.push(DECK_SAVING | (saveResult+1));
    }
}

void loop1() {
    if(rp2040.fifo.pop_nb(&fifoData)) switch(fifoData & 0xFF000000) {
        case DISP_BTN_PRESS:
        case DISP_BTN_RELEASE: if(OLED.display != nullptr) OLED.ButtonsUpdate(fifoData, fifoData & DISP_BTN_RELEASE); break;
        case DISP_PAGE_UPDATE: if(OLED.display != nullptr) OLED.PageUpdate(fifoData & 0xFF); break;
        case DECK_SAVING:
            if(OLED.display != nullptr) OLED.SaveUpdate(fifoData & 0xFF);
            break;
        default: break;
    }

    if(OLED.display != nullptr)
        OLED.IdleOps();
}

void PixelUpdate(const uint32_t &rgb, const int &pixel, const bool &fill)
{
    const uint8_t r = rgb & 0xFF;
    const uint8_t g = (rgb >> 8) & 0xFF;
    const uint8_t b = (rgb >> 16) & 0xFF;
    PixelUpdate(r, g, b, pixel, fill); 
}

void PixelUpdate(const int &r, const int &g, const int &b, const int &pixel, const bool &fill)
{
    if(fill) neopixel.fill(Adafruit_NeoPixel::Color(r, g, b), pixel);
    else neopixel.setPixelColor(pixel, r, g, b);

    neopixel.show();
}