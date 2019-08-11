/*
 * This file is part of the project Fancy Clock
 *
 * This class implements a effect to display the time using multiple colors
 * constantly altering by fading in the next color while fading out the old
 * color.
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

#ifndef FADING_COLORS_H_
#define FADING_COLORS_H_

#include "BaseEffect.h"

class FadingColors : public BaseEffect {
public:
  /*
  * Class constructor
  *
  * Params:
  *   DD: Pointer to the display driver class object
  */
  FadingColors(DisplayDriver *DD);

  void update(unsigned long tnow, bool time_is_synched);

  int nextSubEffect();

private:
  cRGB color; // Color currently used to display the time

  /* For this effect the different sub-effects are just speed levels */
  uint8_t selected_sub_effect;

  uint8_t stage;
  uint8_t speed;
};

#endif //FADING_COLORS_H_
