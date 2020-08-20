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

#define SUB_EFFECT_COUNT 7

DiceLikeDigits::DiceLikeDigits(DisplayDriver *DD) : BaseEffect(DD) {
  selected_sub_effect = 0;
  applySubEffect(selected_sub_effect);
}

void DiceLikeDigits::update(datetime dt, bool time_is_synched, uint8_t dm) {
  uint8_t dv[4];

  blinking_colon_color = getColor(0);

  base_update(dt, time_is_synched, dm);

  getDigitValues(dv, dm);

  setDigit(0, dv[0], getColor(1));
  setDigit(1, dv[1], getColor(2));
  setDigit(2, dv[2], getColor(1));
  setDigit(3, dv[3], getColor(2));

  pDisplayDriver->sync();
}

uint8_t DiceLikeDigits::getNumberOfSubEffects() {
  return SUB_EFFECT_COUNT;
}

int DiceLikeDigits::nextSubEffect() {
  selected_sub_effect = (selected_sub_effect + 1) % SUB_EFFECT_COUNT;
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

void DiceLikeDigits::setDigit(uint8_t index, uint8_t digit, cRGB color) {
  unsigned long long bit_map = getDigitBitMap(digit);
  cRGB value;

  for (int i = 0; i < LEDCountPerDigit; i++) {
    if (bit_map & ((unsigned long long) 1 << i)) {
      value = color;
    } else {
      value.r = value.g = value.b = 0;
    }

    pDisplayDriver->setLED(i + index * LEDCountPerDigit + 2, value);
  }
}

unsigned long long DiceLikeDigits::getDigitBitMap(uint8_t digit) {
  switch (digit) {
    case 1:
      return 0x100000000000;
    case 2:
      return 0x80002;
    case 3:
      return 0x100000080002;
    case 4:
      return 0x2080082;
    case 5:
      return 0x100002080082;
    case 6:
      return 0x2480092;
    case 7:
      return 0x100002480092;
    case 8:
      return 0x29800A6;
    case 9:
      return 0x1000029800A6;
    default:
      return 0x0;
  }
}

cRGB DiceLikeDigits::getColor(uint8_t n) {
  cRGB value;

  switch (n) {
    case 0: // Red
      value.r = 255;
      value.g = 0;
      value.b = 0;
      break;
    case 1: // Green
      value.r = 0;
      value.g = 255;
      value.b = 0;
      break;
    case 2:
      value.r = 0;
      value.g = 0;
      value.b = 255;
      break;
    default:
      value.r = 0;
      value.g = 0;
      value.b = 0;
  }

  return value;
}
