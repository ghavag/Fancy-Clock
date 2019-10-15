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

#include "DiceLikeDigits.h"

#define EFFECT_COUNT 7

DiceLikeDigits::DiceLikeDigits(DisplayDriver *DD) : BaseEffect(DD) {
  selected_sub_effect = 0;
  applySubEffect(selected_sub_effect);
}

void DiceLikeDigits::update(datetime dt, bool time_is_synched, uint8_t dm) {
  base_update(dt, time_is_synched, dm);

  displayCurrentTime(color, dm);

  pDisplayDriver->sync();
}

int DiceLikeDigits::nextSubEffect() {
  selected_sub_effect = (selected_sub_effect + 1) % EFFECT_COUNT;
  applySubEffect(selected_sub_effect);

  return selected_sub_effect;
}

void DiceLikeDigits::applySubEffect(uint8_t sub_eff) {
  switch (sub_eff) {
    // Effect 0 handled by default branch
    case 1:
      color.r = 0;
      color.g = 255;
      color.b = 0;
      break;
    case 2:
      color.r = 0;
      color.g = 0;
      color.b = 255;
      break;
    case 3:
      color.r = 255;
      color.g = 255;
      color.b = 0;
      break;
    case 4:
      color.r = 255;
      color.g = 0;
      color.b = 255;
      break;
    case 5:
      color.r = 0;
      color.g = 255;
      color.b = 255;
      break;
    case 6:
      color.r = 255;
      color.g = 255;
      color.b = 255;
      break;
    default:
      color.r = 255;
      color.g = 0;
      color.b = 0;
  }

  blinking_colon_color = color;
}
