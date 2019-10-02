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

  int nextSubEffect();

protected:
  cRGB color; // Color currently used to display the time
  cRGB fading_in_color[4];
  cRGB fading_out_color[4];

  uint8_t dv[4];
  uint8_t dv_old[4];
  uint8_t dv_last[4];

  //uint8_t selected_sub_effect;

  uint8_t speed;

  void displayCurrentTime(cRGB color, uint8_t dm);

  void setDigit(uint8_t index, uint8_t digit, cRGB color);
};

#endif //FADING_DIGITS_H_
