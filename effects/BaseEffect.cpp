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

#include "BaseEffect.h"
#include <stdlib.h>

void __cxa_pure_virtual(void) {}; // See comment in BaseEffect.h

BaseEffect::BaseEffect(DisplayDriver *DD) {
  pDisplayDriver = DD;
  blinking_colon = true;
  blinking_colon_hpd = 250;
  blinking_colon_color.r = 255;
  blinking_colon_color.g = 0;
  blinking_colon_color.b = 0;
}

void BaseEffect::update(unsigned long tnow, bool time_is_synched, uint8_t dm) {
  blinking_colon = !time_is_synched;
  if (blinking_colon) update_blinking_colon(tnow, dm);
}

void BaseEffect::update_blinking_colon(unsigned long tnow, uint8_t dm) {
  static unsigned char state = 0; // 0 = colon off; 1 = colon on
  static unsigned long tlast = 0;
  cRGB color_upper, color_lower;

  if ((tnow - tlast) > blinking_colon_hpd) {
    tlast = tnow;
    state = (state + 1) % 2;
  }

  if (state || !blinking_colon)
    color_lower = blinking_colon_color;
  else {
    color_lower.r = 0;
    color_lower.g = 0;
    color_lower.b = 0;
  }

  if (dm == DISPLAY_MODE_DM) {
    color_upper.r = 0;
    color_upper.g = 0;
    color_upper.b = 0;
  } else {
    color_upper = color_lower;
  }

  pDisplayDriver->setLED(0, color_lower);
  pDisplayDriver->setLED(1, color_upper);
}

void BaseEffect::getDigitValues(uint8_t *digit_values, uint8_t dm) {
  //static uint8_t digit_values[4];
  tmElements_t tm;

  tm = getTimeOnly();

  switch (dm) {
    case DISPLAY_MODE_MS:
      digit_values[0] = tm.Second % 10;
      digit_values[1] = tm.Second / 10;
      digit_values[2] = tm.Minute % 10;
      digit_values[3] = tm.Minute / 10;
      break;
    case DISPLAY_MODE_DM:
      digit_values[0] = tm.Month % 10;
      digit_values[1] = tm.Month / 10;
      digit_values[2] = tm.Day % 10;
      digit_values[3] = tm.Day / 10;
      break;
    default /* DISPLAY_MODE_HM */:
      digit_values[0] = tm.Minute % 10;
      digit_values[1] = tm.Minute / 10;
      digit_values[2] = tm.Hour % 10;
      digit_values[3] = tm.Hour / 10;
  }
}

void BaseEffect::displayCurrentTime(cRGB color, uint8_t dm) {
  uint8_t dv[4];

  getDigitValues(dv, dm);

  pDisplayDriver->setDigit(0, dv[0], color);
  pDisplayDriver->setDigit(1, dv[1], color);
  pDisplayDriver->setDigit(2, dv[2], color);
  pDisplayDriver->setDigit(3, dv[3], color);
}

cRGB BaseEffect::generateRandomColor() {
  cRGB color;

  color.r = rand() % 256;
  color.g = rand() % 256;
  color.b = rand() % 256;

  return color;
}
