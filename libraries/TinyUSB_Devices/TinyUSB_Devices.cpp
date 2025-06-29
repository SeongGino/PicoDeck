/*
 * This module (hastily) combines Mike Lynch's modified "AbsMouse5" library, and
 * Chris Young's TinyUSB Mouse and Keyboard library (the Keyboard half, anyways),
 * which in itself uses pieces of Arduino's basic Keyboard library.
 */

#ifdef USE_TINYUSB
  #include <Adafruit_TinyUSB.h>
#elifdef CFG_TUSB_MCU
  #error Incompatible USB stack. Use Adafruit TinyUSB.
#else
  #include <HID.h>
#endif

#include "TinyUSB_Devices.h"

/*****************************
 *   GLOBAL SECTION
 *****************************/
Adafruit_USBD_HID usbHid;

enum HID_RID_e{
    HID_RID_KEYBOARD = 1
};

uint8_t desc_hid_report[] = {
    TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(1))
};

void TinyUSBDevices_::begin(int polRate) {
    usbHid.setPollInterval(polRate);
    usbHid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
    usbHid.begin();
}

TinyUSBDevices_ TinyUSBDevices;
  
 /*****************************
 *   KEYBOARD SECTION
 *****************************/ 

  Keyboard_::Keyboard_(void) {
    memset(modsBuffer, 0, sizeof(modsBuffer));
  }
  
  void Keyboard_::report()
  {
    while(!usbHid.ready()) yield();
    usbHid.keyboardReport(HID_RID_KEYBOARD, _keyReport.modifiers, _keyReport.keys);
    TinyUSBDevices.newReport = false;
  }
  
  #define SHIFT 0x80
  const uint8_t _asciimap[128] =
  {
    0x00,             // NUL
    0x00,             // SOH
    0x00,             // STX
    0x00,             // ETX
    0x00,             // EOT
    0x00,             // ENQ
    0x00,             // ACK  
    0x00,             // BEL
    0x2a,     // BS Backspace
    0x2b,     // TAB  Tab
    0x28,     // LF Enter
    0x00,             // VT 
    0x00,             // FF 
    0x00,             // CR 
    0x00,             // SO 
    0x00,             // SI 
    0x00,             // DEL
    0x00,             // DC1
    0x00,             // DC2
    0x00,             // DC3
    0x00,             // DC4
    0x00,             // NAK
    0x00,             // SYN
    0x00,             // ETB
    0x00,             // CAN
    0x00,             // EM 
    0x00,             // SUB
    0x00,             // ESC
    0x00,             // FS 
    0x00,             // GS 
    0x00,             // RS 
    0x00,             // US 
  
    0x2c,      //  ' '
    0x1e|SHIFT,    // !
    0x34|SHIFT,    // "
    0x20|SHIFT,    // #
    0x21|SHIFT,    // $
    0x22|SHIFT,    // %
    0x24|SHIFT,    // &
    0x34,          // '
    0x26|SHIFT,    // (
    0x27|SHIFT,    // )
    0x25|SHIFT,    // *
    0x2e|SHIFT,    // +
    0x36,          // ,
    0x2d,          // -
    0x37,          // .
    0x38,          // /
    0x27,          // 0
    0x1e,          // 1
    0x1f,          // 2
    0x20,          // 3
    0x21,          // 4
    0x22,          // 5
    0x23,          // 6
    0x24,          // 7
    0x25,          // 8
    0x26,          // 9
    0x33|SHIFT,      // :
    0x33,          // ;
    0x36|SHIFT,      // <
    0x2e,          // =
    0x37|SHIFT,      // >
    0x38|SHIFT,      // ?
    0x1f|SHIFT,      // @
    0x04|SHIFT,      // A
    0x05|SHIFT,      // B
    0x06|SHIFT,      // C
    0x07|SHIFT,      // D
    0x08|SHIFT,      // E
    0x09|SHIFT,      // F
    0x0a|SHIFT,      // G
    0x0b|SHIFT,      // H
    0x0c|SHIFT,      // I
    0x0d|SHIFT,      // J
    0x0e|SHIFT,      // K
    0x0f|SHIFT,      // L
    0x10|SHIFT,      // M
    0x11|SHIFT,      // N
    0x12|SHIFT,      // O
    0x13|SHIFT,      // P
    0x14|SHIFT,      // Q
    0x15|SHIFT,      // R
    0x16|SHIFT,      // S
    0x17|SHIFT,      // T
    0x18|SHIFT,      // U
    0x19|SHIFT,      // V
    0x1a|SHIFT,      // W
    0x1b|SHIFT,      // X
    0x1c|SHIFT,      // Y
    0x1d|SHIFT,      // Z
    0x2f,          // [
    0x31,          // bslash
    0x30,          // ]
    0x23|SHIFT,    // ^
    0x2d|SHIFT,    // _
    0x35,          // `
    0x04,          // a
    0x05,          // b
    0x06,          // c
    0x07,          // d
    0x08,          // e
    0x09,          // f
    0x0a,          // g
    0x0b,          // h
    0x0c,          // i
    0x0d,          // j
    0x0e,          // k
    0x0f,          // l
    0x10,          // m
    0x11,          // n
    0x12,          // o
    0x13,          // p
    0x14,          // q
    0x15,          // r
    0x16,          // s
    0x17,          // t
    0x18,          // u
    0x19,          // v
    0x1a,          // w
    0x1b,          // x
    0x1c,          // y
    0x1d,          // z
    0x2f|SHIFT,    // {
    0x31|SHIFT,    // |
    0x30|SHIFT,    // }
    0x35|SHIFT,    // ~
    0       // DEL
  };
  
  // press() adds the specified key (printing, non-printing, or modifier)
  // to the persistent key report and sends the report.  Because of the way 
  // USB HID works, the host acts like the key remains pressed until we 
  // call release(), releaseAll(), or otherwise clear the report and resend.
  bool Keyboard_::press(uint8_t k)
  {
    if (k >= 136) {     // it's a non-printing key (not a modifier)
      k = k - 136;
    } else if (k >= 128) {  // it's a modifier key
      if(_keyReport.modifiers & (1<<(k-128))) ++modsBuffer[k-128];
      else {
        _keyReport.modifiers |= (1<<(k-128));
        TinyUSBDevices.newReport = true;
      }
      return true;
    } else {        // it's a printing key
      k = pgm_read_byte(_asciimap + k);
      if (!k) {
        setWriteError();
        return false;
      }
      if (k & 0x80) {           // it's a capital letter or other character reached with shift
        if(_keyReport.modifiers & 0x02) ++modsBuffer[1];
        else _keyReport.modifiers |= 0x02; // the left shift modifier
        k &= 0x7F;
      }
    }
    
    // Add k to the key report only if it's not already present
    // and if there is an empty slot.
    uint8_t i;
    if (k) {
      for (i=0; i<6; ++i) {
        if (!_keyReport.keys[i]) {
          if(_keyReport.keys[i] == k) keyBuffer.insert(k);
          else {
            _keyReport.keys[i] = k;
            TinyUSBDevices.newReport = true;
          }
          break;
        }
      }

      if (i == 6) {
        setWriteError();
        return false;
      }
    }

    return true;
  }

  // macro for updating modifiers (L/R CTRL/SHIFT/ALT/Meta) with a bitmap
  void Keyboard_::pressModifiers(uint8_t m)
  {
    for(uint8_t i = 0; i < sizeof(modsBuffer); ++i) {
      if(m & (1<<i)) {
        if(_keyReport.modifiers & (1<<i)) ++modsBuffer[i];
        else _keyReport.modifiers |= (1<<i);
      }
    }
    TinyUSBDevices.newReport = true;
  }

  void Keyboard_::releaseModifiers(uint8_t m)
  {
    for(uint8_t i = 0; i < sizeof(modsBuffer); ++i) {
      if(m & (1<<i)) {
        if(modsBuffer[i]) --modsBuffer[i];
        else _keyReport.modifiers &= ~(1<<i);
      }
    }
    TinyUSBDevices.newReport = true;
  }
  
  // release() takes the specified key out of the persistent key report and
  // sends the report.  This tells the OS the key is no longer pressed and that
  // it shouldn't be repeated any more.
  bool Keyboard_::release(uint8_t k)
  {
    uint8_t i;
    if (k >= 136) {     // it's a non-printing key (not a modifier)
      k = k - 136;
    } else if (k >= 128) {  // it's a modifier key
      if(modsBuffer[k-128]) --modsBuffer[k-128];
      else {
        _keyReport.modifiers &= ~(1<<(k-128));
        TinyUSBDevices.newReport = true;
      }
      return true;
    } else {        // it's a printing key
      k = pgm_read_byte(_asciimap + k);
      if (!k) {
        return false;
      }
      if (k & 0x80) {             // it's a capital letter or other character reached with shift
        if(modsBuffer[1]) --modsBuffer[1];
        else _keyReport.modifiers &= ~0x02;  // the left shift modifier
        k &= 0x7F;
      }
    }
    
    // Test the key report to see if k is present.  Clear it if it exists.
    for (i=0; i<6; ++i) {
      if (0 != k && _keyReport.keys[i] == k) {
        auto matchingKey = keyBuffer.find(k);
        if(matchingKey != keyBuffer.end()) keyBuffer.erase(matchingKey);
        else {
          _keyReport.keys[i] = 0x00;
          TinyUSBDevices.newReport = true;
        }
        break;
      }
    }

    return true;
  }
  
  void Keyboard_::releaseAll(void)
  {
    if(memcmp(_keyReport.keys, (char[]){0,0,0,0,0,0}, sizeof(KeyReport::keys)) || _keyReport.modifiers) {
      _keyReport.keys[0] = 0;
      _keyReport.keys[1] = 0;
      _keyReport.keys[2] = 0;
      _keyReport.keys[3] = 0;
      _keyReport.keys[4] = 0;
      _keyReport.keys[5] = 0;
      _keyReport.modifiers = 0;
      memset(modsBuffer, 0, sizeof(modsBuffer));
      keyBuffer.clear();
      TinyUSBDevices.newReport = true;
    }
  }

  size_t Keyboard_::write(uint8_t c)
  {
    // stub
    return 1;
  }

  size_t Keyboard_::write(const uint8_t *buffer, size_t size) {
    // stub
    return 1;
  }
  
  Keyboard_ Keyboard;//create an instance of the Keyboard object
