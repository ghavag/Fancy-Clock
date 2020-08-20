/*
 * This file is part of the project Fancy Clock
 *
 * This class implements an effect which, if a digit changes, fades the old
 * digit out smoothly while fading in the new digit.
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

#ifndef FADING_DIGITS_H_
#define FADING_DIGITS_H_

#include "BaseEffect.h"

class FadingDigits : public BaseEffect {
public:
  /*
  * Class constructor
  *
  * Params:
  *   DD: Pointer to the display driver class object
  */
  FadingDigits(DisplayDriver *DD);

  void update(datetime dt, bool time_is_synched, uint8_t dm);

  uint8_t getNumberOfSubEffects();

  void applySubEffect(uint8_t sub_eff);

  int nextSubEffect();

protected:
  uint8_t selected_sub_effect; // Index of the current selected sub-effect
  cRGB color; // Color currently used to display the time

  /* For each digit three color values are hold */
  cRGB fading_in_color[4]; // LED colors of the current digits
  cRGB fading_out_color[4]; // LED colors of the previous digits
  cRGB both_color[4]; // LED colors belongig to the current and previous digit

  /* Variables to help track the digit transmissions */
  uint8_t dv[4];
  uint8_t dv_old[4];
  uint8_t dv_last[4];

  void displayCurrentTime(cRGB color, uint8_t dm);

  void setDigit(uint8_t index, uint8_t digit, cRGB color);
};

#endif //FADING_DIGITS_H_
