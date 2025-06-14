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
    // wait for signal from Core1, which should be after prefs have loaded
    rp2040.fifo.pop();

    // In case some I2C devices deadlock the program
    // (can happen due to bad pin mappings)
    Wire.setTimeout(100);
    Wire1.setTimeout(100);

    if(OLED.Begin(DISP_SCL, DISP_SDA, Adafruit_MultiDisplay::I2C_SSD1306) == false) {
        // Does this ever actually happen...?
        #ifdef SERIAL_DEBUG
        Serial.println("Display init error!");
        #endif // SERIAL_DEBUG
    }
}

void setup1()
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

    buttons.Begin();
    #ifndef SERIAL_DEBUG
    buttons.ReportEnable();
    #endif // SERIAL_DEBUG

    DeckCommon::Prefs = new DeckPrefs();
    buttons.page = DeckCommon::Prefs->curPage;
    rp2040.fifo.push(0);

    neopixel.begin();
    
    switch(DeckCommon::Prefs->curPage) {
    case 0:  PixelUpdate(255, 0, 0, 0, true); break;
    case 1:  PixelUpdate(0, 255, 0, 0, true); break;
    case 2:  PixelUpdate(0, 0, 255, 0, true); break;
    }
}

void loop() {
    if(OLED.display != nullptr)
        OLED.IdleOps();

    if(rp2040.fifo.pop_nb(&fifoData)) switch(fifoData & 0xFF000000) {
        case DISP_BTN_UPDATE: if(OLED.display != nullptr) OLED.ButtonsUpdate(fifoData); break;
        case DISP_PAGE_UPDATE:  if(OLED.display != nullptr) OLED.PageUpdate(fifoData & 0x000000FF); break;
        default: break;
    }
}

void loop1() {
    buttons.Poll(0);

    if(buttons.pressed) {
        rp2040.fifo.push(buttons.pressed);
        #ifdef SERIAL_DEBUG
        for(int i = 0; i < ButtonCount; ++i) if(buttons.pressed & 1 << i)
            Serial.printf("Pressed Button %d (Key: %d)\n", i+1, *&LightgunButtons::ButtonDesc[i].reportCode+buttons.page);
        #endif // SERIAL_DEBUG
    }
    if(buttons.released)
        rp2040.fifo.push(buttons.released);

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
        switch(DeckCommon::Prefs->curPage) {
            case 0: PixelUpdate(255, 0, 0, 0, true); break;
            case 1: PixelUpdate(0, 255, 0, 0, true); break;
            case 2: PixelUpdate(0, 0, 255, 0, true); break;
            default: break;
        }
        
        canSave = true;
        lastSaveChecked = millis();
    }

    if(millis() - lastSaveChecked >= SAVE_INTERVAL) {
        lastSaveChecked = millis();
        if(canSave) {
            canSave = false;
            DeckCommon::Prefs->Save();
        }
    }
}

void PixelUpdate(const int &r, const int &g, const int &b, const int &pixel, const bool &fill)
{
    if(fill) neopixel.fill(Adafruit_NeoPixel::Color(r, g, b), pixel);
    else neopixel.setPixelColor(pixel, r, g, b);

    neopixel.show();
}