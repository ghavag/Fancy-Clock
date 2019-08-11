/*
 * This file is part of the project Fancy Clock
 *
 * The time is displayed using various effets. Each effect is implemented in its
 * own class. Every such class needs to inherit from this (BaseEffect) class.
 *
 * Copyright (c) 2019 Alexander Graeb
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * (see LICENSE_LGPLv3) along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 */

#ifndef BASE_EFFECT_H_
#define BASE_EFFECT_H_

#include "../time.h"
#include "../DisplayDriver.h"

/*
* Seems that AVR-GCC does not correctly implement pure virtual functions (see
* https://www.avrfreaks.net/comment/341297#comment-341297). The implementation
* is located in BaseEffect.cpp.
*/
extern "C" void __cxa_pure_virtual(void);

class BaseEffect {
public:
  /*
  * If the colon is set to blink, blinking_colon_hpd is the half period
  * duration (hpd) in milliseconds. If set to 500 for example, the colon lights
  * up for 500 ms, then turns off for another 500 ms and so on.
  */
  unsigned int blinking_colon_hpd;

  /*
  * cRGB value representing the color in which the colon blinks.
  */
  cRGB blinking_colon_color;

  /*
  * Class constructor
  *
  * Params:
  *   DD: Pointer to the display driver class object
  */
  BaseEffect(DisplayDriver *DD);

  /*
  * This method should be called regulary (i. e. every few milliseconds). It
  * will update the display according to the effects behaviour.
  *
  * tnow: The current time in milliseconds.
  * time_is_synched: True if time is in sync with DCF77. Most effects let the
  *                  colon blink to signal that the time is not synced.
  */
  virtual void update(unsigned long tnow, bool time_is_synched);

  /*
  * Turn colon blinking on or off.
  *
  * blink: True lets the colon blink, false turn colon on constantly.
  */
  virtual void setBlinkingColon(bool blink);

  /*
  * Effects may implement some sub-effects. If this method is called, the
  * effect shall switch to the next sub-effect. If the last sub-effect is
  * selected, the first sub-effect shall be selected again.
  *
  * return: The number of the selected sub-effect starting with zero.
  */
  virtual int nextSubEffect() = 0;

protected:
  /* Pointer to the display driver */
  DisplayDriver *pDisplayDriver;

  /*
  * A blinking colon indicates that the time is not synced to DCF77 (anymore).
  * Each effect should let the colon blink if this variable is set to true.
  */
  bool blinking_colon;

  /*
  * This method is usually called from the update() method if blinking_colon is
  * true. This method lets the colon blink.
  *
  * tnow: The current time in milliseconds.
  */
  virtual void update_blinking_colon(unsigned long tnow);

  /*
  * Set the display to the current time.
  *
  * color: Color in which the time should be displayed.
  */
  virtual void displayCurrentTime(cRGB color);

  /*
  * Generate a random color value. The user must initialize the random number
  * generator (by e. g. calling srand()).
  *
  * return: A random color as struct cRGB.
  */
  virtual cRGB generateRandomColor();
};

#endif //BASE_EFFECT_H_
