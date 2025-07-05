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

#include "ColorExplosion.h"

#include <stdlib.h>

#define SUB_EFFECT_COUNT 3

ColorExplosion::ColorExplosion(DisplayDriver *DD) : BaseEffect(DD) {
  k = 0;
  applySubEffect(0);
}

uint8_t ColorExplosion::getNumberOfSubEffects() {
  return SUB_EFFECT_COUNT;
}

void ColorExplosion::applySubEffect(uint8_t sub_eff) {
  // How often are the colors re-calculated?
  switch (sub_eff) {
    case 1:
      delay = 32; // Slow i.e. every second
      break;
    case 2:
      delay = 0; // Only on digit change
      break;
    default:
      delay = 1; // Very fast
  }

  selected_sub_effect = sub_eff;
}

int ColorExplosion::nextSubEffect() {
  applySubEffect((selected_sub_effect + 1) % SUB_EFFECT_COUNT);

  return selected_sub_effect;
}

void ColorExplosion::update(datetime dt, bool time_is_synched, uint8_t dm) {
  uint8_t dv[4];
  static uint8_t old_max_brightness;

  base_update(dt, time_is_synched, dm);
  getDigitValues(dv, dm);

  if (old_max_brightness != pDisplayDriver->getMaxBrightness()) {
    k = 0;
    old_max_brightness = pDisplayDriver->getMaxBrightness();
  } else if (delay == 0) {
    k = 1;

    for (uint8_t i = 0; i < 4; i++) {
      if (dv[i] != old_dv[i]) {
        old_dv[i] = dv[i];
        k = 0;
      }
    }
  } else {
    k = (k+1) % delay;
  }


  if (k == 0) {
    color_upper = generateRandomColor();
    color_lower = generateRandomColor();

    setDigit(0, dv[0]);
    setDigit(1, dv[1]);
    setDigit(2, dv[2]);
    setDigit(3, dv[3]);
  }

  pDisplayDriver->sync();
}

void ColorExplosion::select() {
  BaseEffect::select();

  /*
  * Set to a value that is never displayed to force update the display next
  * time update() is called for the case the display gets only updated if a
  * digit has changed (delay = 0).
  */
  old_dv[0] = 255;

  if (delay > 1) k = delay - 1;
}

void ColorExplosion::update_blinking_colon() {
  static unsigned char state = 0; // 0 = colon off; 1 = colon on
  static unsigned long tlast = 0;
  unsigned long tnow;
  cRGB color_upper, color_lower;

  tnow = millis();

  if ((tnow - tlast) > blinking_colon_hpd) {
    tlast = tnow;
    state = (state + 1) % 2;
  }

  if (state || !blinking_colon) {
    color_lower = this->color_upper;
    color_upper = this->color_lower;
  } else {
    color_lower.r = 0;
    color_lower.g = 0;
    color_lower.b = 0;
    color_upper = color_lower;
  }

  pDisplayDriver->setLED(0, color_lower);
  pDisplayDriver->setLED(1, color_upper);
}

void ColorExplosion::setDigit(uint8_t index, uint8_t digit) {
  unsigned long long bit_map = pDisplayDriver->getDigitBitMap(digit);
  cRGB value;

  for (int i = 0; i < LEDCountPerDigit; i++) {
    if (bit_map & ((unsigned long long) 1 << i)) {
      value = generateRandomColor();
    } else {
      value.r = value.g = value.b = 0;
    }

    pDisplayDriver->setLED(i + index * LEDCountPerDigit + 2, value);
  }
}

cRGB ColorExplosion::generateRandomColor() {
  return BaseEffect::generateRandomColor();
}
