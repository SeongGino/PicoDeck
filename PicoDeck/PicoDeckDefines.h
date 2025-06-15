 /*!
 * @file PicoDeckDefines.h
 * @brief Global precompiler definitions & build options for the PicoDeck project.
 *
 * @copyright That One Seong, 2025
 * @copyright GNU General Public License
 */ 

#pragma once

// Uncomment to disable reporting and output presses to Serial instead.
//#define SERIAL_DEBUG

#define MANUFACTURER_NAME "Seong's"
#define DEVICE_NAME "PicoDeck"
#define DEVICE_VID 0x0920
#define DEVICE_PID 0x1998

#define NEOPIXEL_PIN 16

// to keep orig display pin determination code
#define DISP_SDA 18
#define DISP_SCL 19