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

#include "FadingDigits.h"

#define FADING_DIGITS_SPEED 4

FadingDigits::FadingDigits(DisplayDriver *DD) : BaseEffect(DD) {
  speed = 1;

  color.r = 0;
  color.g = 0;
  color.b = 0;

  for (int i = 0; i < 4; i++) {
    fading_in_color[i].r = 0;
    fading_in_color[i].g = 0;
    fading_in_color[i].b = 0;

    fading_out_color[i].r = 0;
    fading_out_color[i].g = 0;
    fading_out_color[i].b = 0;
  }
}

void FadingDigits::update(datetime dt, bool time_is_synched, uint8_t dm) {
  base_update(dt, time_is_synched, dm);

  displayCurrentTime(color, dm);

  pDisplayDriver->sync();
}

int FadingDigits::nextSubEffect() {
  return 0;
}

void FadingDigits::displayCurrentTime(cRGB color, uint8_t dm) {
  getDigitValues(dv, dm);

  for (int i = 0; i < 4; i++) {
    if (dv[i] != dv_old[i]) {
      dv_last[i] = dv_old[i];
      dv_old[i] = dv[i];
      fading_out_color[i] = fading_in_color[i];
      fading_in_color[i].g = 0;
    } else if (fading_in_color[i].g < (255 - FADING_DIGITS_SPEED)) {
      fading_in_color[i].g += FADING_DIGITS_SPEED;
    } else {
      fading_in_color[i].g = 255;
    }

    if (fading_out_color[i].g > FADING_DIGITS_SPEED)
      fading_out_color[i].g -= FADING_DIGITS_SPEED;
    else
      fading_out_color[i].g = 0;
  }

  setDigit(0, dv[0], color);
  setDigit(1, dv[1], color);
  setDigit(2, dv[2], color);
  setDigit(3, dv[3], color);
}

void FadingDigits::setDigit(uint8_t index, uint8_t digit, cRGB color) {
  unsigned long long bit_map_new = pDisplayDriver->getDigitBitMap(dv[index]);
  unsigned long long bit_map_last = pDisplayDriver->getDigitBitMap(dv_last[index]);
  cRGB value;

  for (int i = 0; i < LEDCountPerDigit; i++) {
    //value = pDisplayDriver->getLED(i + index * LEDCountPerDigit + 2);

    if (bit_map_new & ((unsigned long long) 1 << i)) {
      value = fading_in_color[index];
    } else if (bit_map_last & ((unsigned long long) 1 << i)) {
      value = fading_out_color[index];
    } else {
      value.r = value.g = value.b = 0;
    }

    pDisplayDriver->setLED(i + index * LEDCountPerDigit + 2, value);
  }
}
