/*!
 * @file LightgunButtons.cpp
 * @brief HID buttons originally intended for use with a light gun.
 *
 * @copyright Mike Lynch, 2021
 *
 *  LightgunButtons is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <Arduino.h>
#include "LightgunButtons.h"
#include <TinyUSB_Devices.h>

LightgunButtons::LightgunButtons(Data_t _data, unsigned int _count) :
    pressed(0),
    released(0),
    repeat(0),
    debounced(0),
    debouncing(0),
    pressedReleased(0),
    interval(33),
    report(0),
    lastMillis(0),
    lastRepeatMillis(0),
    pinState(0xFFFFFFFF),
    internalPressedReleased(0),
    reportedPressed(0),
    count(_count),
    pagesCount(0),
    stateFifo(_data.pArrFifo),
    debounceCount(_data.pArrDebounceCount)
{
}

int LightgunButtons::Begin()
{
    // set button pins to input with pullup
    for(unsigned int i = 0; i < count; ++i) {
        // do no setup if the pin is uninitialized
        if(ButtonDesc[i].pin >= 0) {
            pinMode(ButtonDesc[i].pin, INPUT_PULLUP);
            stateFifo[i] = 0xFFFFFFFF;
            debounceCount[i] = 0;

            if(ButtonDesc[i].keys.size() > (uint)pagesCount)
                pagesCount = ButtonDesc[i].keys.size();
        }
    }
    return pagesCount;
}

void LightgunButtons::Unset()
{
    // set button pins to normal input
    for(unsigned int i = 0; i < count; ++i) {
        // do no setup if the pin is uninitialized
        if(ButtonDesc[i].pin >= 0) {
            pinMode(ButtonDesc[i].pin, INPUT);
            debounceCount[i] = 0;
        }
    }
    pressed = 0;
    released = 0;
    debounced = 0;
    debouncing = 0;
    pressedReleased = 0;
    lastMillis = 0;
    lastRepeatMillis = 0;
    internalPressedReleased = 0;
    reportedPressed = 0;
    pagesCount = 0;
}

uint32_t LightgunButtons::Poll(unsigned long minTicks)
{
    unsigned long m = millis();
    unsigned long ticks = m - lastMillis;
    uint32_t bitMask;
    
    // reset pressed and released from last poll
    pressed = 0;
    released = 0;
    pressedReleased = 0;
    
    if(ticks < minTicks) {
        return 0;
    }
    lastMillis = m;

    if(debouncing && ticks) {
        bitMask = 1;
        for(unsigned int i = 0; i < count; ++i, bitMask <<= 1) {
            if(ButtonDesc[i].pin >= 0) {
                if(debounceCount[i]) {
                    if(ticks < debounceCount[i]) {
                        debounceCount[i] -= ticks;
                    } else {
                        debounceCount[i] = 0;
                        debouncing &= ~bitMask;
                    }
                }
            }
        }
    }

    bitMask = 1;
    for(unsigned int i = 0; i < count; ++i, bitMask <<= 1) {
        const Desc_t& btn = ButtonDesc[i];

        // do no processing if the pin is uninitialized
        if(btn.pin >= 0) {
            // if not debouncing
            if(!debounceCount[i]) {
                // read the pin, expected to return 0 or 1
                uint32_t state = digitalRead(btn.pin);

                // add the state to the fifo
                stateFifo[i] <<= 1;
                stateFifo[i] |= state;

                // apply the mask and check the value
                uint32_t m = stateFifo[i] & BTN_AG_MASK;
                if(!m) {
                    state = 0;
                } else if(m == BTN_AG_MASK) {
                    // use the bit mask for this button
                    state = bitMask;
                } else {
                    // button is bouncing, continue to next button
                    continue;
                }

                // if existing pin state does not match new state
                if((pinState & bitMask) != state) {
                    // update the pin state
                    pinState = (pinState & ~bitMask) | state;

                    // set the debounce counter and set the flag
                    debounceCount[i] = DEBOUNCE_TICKS;
                    debouncing |= bitMask;

                    if(!state) {
                        // state is low, button is pressed

                        if((btn.keys.at(0) & 0xFF) < LGB_PAGEKEYS) {
                            Keyboard.releaseAll();
                            switch(btn.keys.at(0) & 0xFF) {
                            case LGB_PREV: if(page) --page; break;
                            case LGB_NEXT: if(page < pagesCount-1) ++page; break;
                            default: break;
                            }
                        }

                        // if reporting is enabled for the button
                        if(report & bitMask) {
                            if(btn.keys.size() > (uint)page) {
                                if(btn.keys.at(page) & 0xFF00)
                                    Keyboard.pressModifiers(btn.keys.at(page) >> 8);

                                if((btn.keys.at(page) & 0xFF) > LGB_PAGEKEYS)
                                    Keyboard.press(btn.keys.at(page) & 0xFF);
                            }
                            reportedPressed |= bitMask;
                        }

                        // button is debounced pressed and add it to the pressed/released combo
                        debounced |= bitMask;
                        pressed |= bitMask;
                        internalPressedReleased |= bitMask;

                    } else {
                        // state high, button is not pressed

                        // if the button press was reported then report the release
                        // note that the report flag is ignored here to avoid stuck buttons
                        // in case the reporting is disabled while button(s) are pressed
                        if(reportedPressed & bitMask) {
                            reportedPressed &= ~bitMask;

                            if(btn.keys.size() > (uint)page) {
                                if(btn.keys.at(page) & 0xFF00)
                                    Keyboard.releaseModifiers(btn.keys.at(page) >> 8);

                                if((btn.keys.at(page) & 0xFF) > LGB_PAGEKEYS)
                                    Keyboard.release(btn.keys.at(page));
                            }
                        }

                        // clear the debounced state and button is released
                        debounced &= ~bitMask;
                        released |= bitMask;

                        // if all buttons released
                        if(!debounced) {
                            // report the combination pressed/released state
                            pressedReleased = internalPressedReleased;
                            internalPressedReleased = 0;
                        }
                    }
                }
            }
        }
    }

    return pressed;
}

void LightgunButtons::SendReports()
{
    if(TinyUSBDevices.newReport)
        Keyboard.report();
}

void LightgunButtons::ReleaseAll()
{
    Keyboard.releaseAll();
    SendReports();
}

uint32_t LightgunButtons::Repeat()
{
    unsigned long m = millis();
    if(m - lastRepeatMillis >= interval) {
        lastRepeatMillis = m;
        repeat = debounced;
    } else {
        repeat = 0;
    }
    return repeat;
}
