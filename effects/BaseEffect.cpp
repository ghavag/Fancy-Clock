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

void BaseEffect::update(unsigned long tnow, bool time_is_synched) {
  blinking_colon = !time_is_synched;
  if (blinking_colon) update_blinking_colon(tnow);
}

void BaseEffect::update_blinking_colon(unsigned long tnow) {
  static unsigned char state = 0; // 0 = colon off; 1 = colon on
  static unsigned long tlast = 0;
  cRGB color;

  if ((tnow - tlast) > blinking_colon_hpd) {
    tlast = tnow;
    state = (state + 1) % 2;
  }

  if (state || !blinking_colon)
    color = blinking_colon_color;
  else {
    color.r = 0;
    color.g = 0;
    color.b = 0;
  }

  pDisplayDriver->setLED(0, color);
  pDisplayDriver->setLED(1, color);
}

void BaseEffect::setBlinkingColon(bool blink) {
  /*if (!blink) {
    pDisplayDriver->setLED(0, blinking_colon_color);
    pDisplayDriver->setLED(1, blinking_colon_color);
  }*/

  blinking_colon = blink;
}

void BaseEffect::displayCurrentTime(cRGB color) {
  tmElements_t tm;

  tm = getTimeOnly();
  pDisplayDriver->setDigit(0, tm.Minute % 10, color);
  pDisplayDriver->setDigit(1, tm.Minute / 10, color);
  pDisplayDriver->setDigit(2, tm.Hour % 10, color);
  pDisplayDriver->setDigit(3, tm.Hour / 10, color);
}

cRGB BaseEffect::generateRandomColor() {
  cRGB color;

  color.r = rand() % 256;
  color.g = rand() % 256;
  color.b = rand() % 256;

  return color;
}
