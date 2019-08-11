/*
 * This file is part of the project Fancy Clock
 *
 * This class implements a effect displaying each digit and the colon in a
 * random generated color. The color of a specific digit changes every time the
 * displayed number has changed.
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

#ifndef RANDOM_COLORED_DIGIT_H_
#define RANDOM_COLORED_DIGIT_H_

#include "BaseEffect.h"

class RandomColoredDigit : public BaseEffect {
public:
  /*
  * Class constructor
  *
  * Params:
  *   DD: Pointer to the display driver class object
  */
  RandomColoredDigit(DisplayDriver *DD);

  void update(unsigned long tnow, bool time_is_synched);

  /*
  * This effect doesn't implement sub-effects but instead generates completely
  * new colors if this method is called. Returns always 0.
  */
  int nextSubEffect();

private:
  /* Colors used for the digits. */
  cRGB color_digit[4];
};

#endif //RANDOM_COLORED_DIGIT_H_
