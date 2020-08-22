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

#define DISPLAY_MODE_HM 0 // Display Hours : Minutes
#define DISPLAY_MODE_MS 1 // Display Minutes : Seconds
#define DISPLAY_MODE_DM 2 // Display Day : Month
#define DISPLAY_MODES 3 // Numer of display modes

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
  * Structure to pass date and time on update method (see below) call.
  */
  struct datetime {
    // Time
    uint8_t Seconds:4;      // low decimal digit 0-9
    uint8_t Seconds10:3;    // high decimal digit 0-5
    uint8_t Minutes:4;
    uint8_t Minutes10:3;
    uint8_t Hour:4;
    uint8_t Hour10:2;
    // Date (without the year)
    uint8_t Date:4;           // Day of month, 1 = first day
    uint8_t Date10:2;
    uint8_t Month:4;          // Month, 1 = January
    uint8_t Month10:1;
  };

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
  * dt: Struct to pass date and time.
  * time_is_synched: True if time is in sync with DCF77. Most effects let the
  *                  colon blink to signal that the time is not synced.
  * dm: Display mode (show time as hh:mm or mm:ss and so on)
  */
  virtual void update(datetime dt, bool time_is_synched, uint8_t dm) = 0;

  /*
  * Returns the number of sub-effects a effect has.
  *
  * return:
  *   = 0: No or unspecific number of sub-effects
  *   > 0: Number of sub-effects
  */
  virtual uint8_t getNumberOfSubEffects();

  /*
  * Select a sub-effect
  *
  * sub_eff: The number of the sub-effect starting from 0
  */
  virtual void applySubEffect(uint8_t sub_eff);

  /*
  * Effects may implement some sub-effects. If this method is called, the
  * effect shall switch to the next sub-effect. If the last sub-effect is
  * selected, the first sub-effect shall be selected again.
  *
  * return: The number of the selected sub-effect starting with zero.
  */
  virtual int nextSubEffect();

  /*
  * Sould be called on effect switch to notify the new effect that it has just
  * been selected. For example some effects may update the display only
  * occasionally so that the display still shows the content of the previous
  * effect. In that case calling this method would force update the display.
  */
  virtual void select();

protected:
  /* Pointer to the display driver */
  DisplayDriver *pDisplayDriver;

  /*
  * A blinking colon indicates that the time is not synced to DCF77 (anymore).
  * Each effect should let the colon blink if this variable is set to true.
  */
  static bool blinking_colon;

  static datetime dt; // Struct var that holds date and time to be displayed.

  static uint8_t dm; // Display mode. Indicates how date and time is displayed.

  /*
  * Does stuff which pretty much is done in every sub-effect when the public
  * update method is called, i. e. this method is called from the update method
  * in all sub-effects.
  *
  * This method shared the same parameters with the update method (see above).
  */
  void base_update(datetime dt, bool time_is_synched, uint8_t dm);

  /*
  * Let the colon blink. This method is usually called from the update() method
  * if blinking_colon is true.
  */
  virtual void update_blinking_colon();

  /*
  * Gets the time and splits it up into four pieces (one for each digit)
  * according to the choosen display mode. The time pieces are stored into an
  * four elements array (one element per hardware digit).
  *
  * digit_values: A pointer of an four elements array of type uint8_t which gets
  *               filled with the time pieces.
  * dm: Display mode according to which the array gets filled (e. g.  hh:mm,
  *     mm:ss and so on).
  */
  void getDigitValues(uint8_t *digit_values, uint8_t dm);

  /*
  * Set the display to the current time.
  *
  * color: Color in which the time should be displayed.
  * dm: Display mode (show hh:mm, mm:ss and so on).
  */
  virtual void displayCurrentTime(cRGB color, uint8_t dm);

  /*
  * Generate a random color value.
  *
  * return: A random color as struct cRGB.
  */
  virtual cRGB generateRandomColor();
};

#endif //BASE_EFFECT_H_
