/*
 * This file is part of the project Fancy Clock
 *
 * The light of the Fancy Clocks display is, even at the lowest level of
 * brightness, very intense. Something what you may not wish to have in e. g.
 * your bedroom. This effect displays the time using less LEDs in a dice like
 * manner.
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

#ifndef DICE_LIKE_DIGITS_H_
#define DICE_LIKE_DIGITS_H_

#include "BaseEffect.h"

class DiceLikeDigits : public BaseEffect {
public:
  /*
  * Class constructor
  *
  * Params:
  *   DD: Pointer to the display driver class object
  */
  DiceLikeDigits(DisplayDriver *DD);

  void update(datetime dt, bool time_is_synched, uint8_t dm);

private:
  uint8_t selected_sub_effect; // Index of the current selected sub-effect

  void setDigit(uint8_t index, uint8_t digit, cRGB color);

  unsigned long long getDigitBitMap(uint8_t digit);

  cRGB getColor(uint8_t n);
};

#endif //DICE_LIKE_DIGITS_H_
