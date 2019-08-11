/*
 * This file is part of the project Fancy Clock
 * For more information see the corresponding header file.
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

#include "FadingColors.h"

#define EFFECT_COUNT 2

FadingColors::FadingColors(DisplayDriver *DD) : BaseEffect(DD) {
  selected_sub_effect = 0; // Sub-effect 0 equals speed = 1
  stage = 0;
  speed = 1;

  // Starting with pure red
  color.r = 255;
  color.g = color.b = 0;
}

void FadingColors::update(unsigned long tnow, bool time_is_synched, uint8_t dm) {
  blinking_colon = !time_is_synched;

  switch (stage) {
    case 0: // Red => green
      if (color.r >= speed) color.r -= speed;
      else color.r = 0;

      if (color.g < (0xFF - speed)) color.g += speed;
      else color.g = 0xFF;

      if (color.r == 0) stage++;
      break;
    case 1: // Green => blue
      if (color.g >= speed) color.g -= speed;
      else color.g = 0;

      if (color.b < (0xFF - speed)) color.b += speed;
      else color.b = 0xFF;

      if (color.g == 0) stage++;
      break;
    case 2: // Blue => red
      if (color.b >= speed) color.b -= speed;
      else color.b = 0;

      if (color.r < (0xFF - speed)) color.r += speed;
      else color.r = 0xFF;

      if (color.b == 0) stage = 0;
      break;
  }

  blinking_colon_color = color;
  update_blinking_colon(tnow, dm);

  displayCurrentTime(color, dm);

  pDisplayDriver->sync();
}

int FadingColors::nextSubEffect() {
  selected_sub_effect = (selected_sub_effect + 1) % 2;

  if (selected_sub_effect == 1) speed = 10;
  else speed = 1;

  return selected_sub_effect;
}
