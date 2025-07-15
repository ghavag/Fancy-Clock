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
#include <stdlib.h>

// 8 gives a transmission time of max. 1 second (1/32 * 256/8)
#define FADING_DIGITS_SPEED 8

#define SUB_EFFECT_COUNT 10

FadingDigits::FadingDigits(DisplayDriver *DD) : BaseEffect(DD) {
  selected_sub_effect = 0;
  applySubEffect(selected_sub_effect);

  for (int i = 0; i < 4; i++) {
    fading_in_color[i].r = 0;
    fading_in_color[i].g = 0;
    fading_in_color[i].b = 0;

    fading_out_color[i].r = 0;
    fading_out_color[i].g = 0;
    fading_out_color[i].b = 0;

    both_color[i].r = 0;
    both_color[i].g = 0;
    both_color[i].b = 0;
  }
}

void FadingDigits::update(datetime dt, bool time_is_synched, uint8_t dm) {
  base_update(dt, time_is_synched, dm);

  displayCurrentTime(color, dm);

  pDisplayDriver->sync();
}

uint8_t FadingDigits::getNumberOfSubEffects() {
  return SUB_EFFECT_COUNT;
}

int FadingDigits::nextSubEffect() {
  selected_sub_effect = (selected_sub_effect + 1) % SUB_EFFECT_COUNT;
  applySubEffect(selected_sub_effect);

  return selected_sub_effect;
}

void FadingDigits::applySubEffect(uint8_t sub_eff) {
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
    case 7:
    case 8:
    case 9:
      color = generateRandomColor();
      break;
    default:
      color.r = 255;
      color.g = 0;
      color.b = 0;
  }

  blinking_colon_color = color;
}

void FadingDigits::displayCurrentTime(cRGB color, uint8_t dm) {
  getDigitValues(dv, dm);

  for (int i = 0; i < 4; i++) {
    if (dv[i] != dv_old[i]) {
      dv_last[i] = dv_old[i];
      dv_old[i] = dv[i];
      both_color[i] = fading_in_color[i];
      fading_out_color[i] = fading_in_color[i];
      fading_in_color[i].g = 0;
    } else {
      /* Update fading in color */
      if (fading_in_color[i].r < (color.r - FADING_DIGITS_SPEED)) {
        fading_in_color[i].r += FADING_DIGITS_SPEED;
      } else {
        fading_in_color[i].r = color.r;
      }

      if (fading_in_color[i].g < (color.g - FADING_DIGITS_SPEED)) {
        fading_in_color[i].g += FADING_DIGITS_SPEED;
      } else {
        fading_in_color[i].g = color.g;
      }

      if (fading_in_color[i].b < (color.b - FADING_DIGITS_SPEED)) {
        fading_in_color[i].b += FADING_DIGITS_SPEED;
      } else {
        fading_in_color[i].b = color.b;
      }

      /* Update both color */
      if (both_color[i].r < (color.r - FADING_DIGITS_SPEED)) {
        both_color[i].r += FADING_DIGITS_SPEED;
      } else {
        both_color[i].r = color.r;
      }

      if (both_color[i].g < (color.g - FADING_DIGITS_SPEED)) {
        both_color[i].g += FADING_DIGITS_SPEED;
      } else {
        both_color[i].g = color.g;
      }

      if (both_color[i].b < (color.b - FADING_DIGITS_SPEED)) {
        both_color[i].b += FADING_DIGITS_SPEED;
      } else {
        both_color[i].b = color.b;
      }

      /* Update fading out color */
      if (fading_out_color[i].r > FADING_DIGITS_SPEED) {
        fading_out_color[i].r -= FADING_DIGITS_SPEED;
      } else {
        fading_out_color[i].r = 0;
      }

      if (fading_out_color[i].g > FADING_DIGITS_SPEED) {
        fading_out_color[i].g -= FADING_DIGITS_SPEED;
      } else {
        fading_out_color[i].g = 0;
      }

      if (fading_out_color[i].b > FADING_DIGITS_SPEED) {
        fading_out_color[i].b -= FADING_DIGITS_SPEED;
      } else {
        fading_out_color[i].b = 0;
      }
    }
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

    // The current LED belongs to the current digit
    if (bit_map_new & ((unsigned long long) 1 << i)) {
      // The current LED belongs to the previous digit as well
      if (bit_map_last & ((unsigned long long) 1 << i)) {
        value = both_color[index];
      }
      // The current LED belongs only to the current digit
      else {
        value = fading_in_color[index];
      }
    }
    // The current LED belongs to the previous digit
    else if (bit_map_last & ((unsigned long long) 1 << i)) {
      value = fading_out_color[index];
    }
    // The current LED belongs neither to the previous nor to the current digit
    else {
      value.r = value.g = value.b = 0;
    }

    pDisplayDriver->setLED(i + index * LEDCountPerDigit + 2, value);
  }
}
