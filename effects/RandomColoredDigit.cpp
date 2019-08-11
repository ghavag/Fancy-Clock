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

#include "RandomColoredDigit.h"
#include <stdlib.h>

RandomColoredDigit::RandomColoredDigit(DisplayDriver *DD) : BaseEffect(DD) {
  nextSubEffect();
}

void RandomColoredDigit::update(unsigned long tnow, bool time_is_synched, uint8_t dm) {
  blinking_colon = !time_is_synched;
  update_blinking_colon(tnow, dm);
  tmElements_t tm;
  uint8_t digit[4];
  static uint8_t old_digit[4];

  getDigitValues(digit, dm);

  for (int i = 0; i < DigitCount; i++) {
    /* Generate new colors if digit has changed */
    if (old_digit[i] != digit[i]) {
      color_digit[i] = generateRandomColor();
      old_digit[i] = digit[i];
    }

    pDisplayDriver->setDigit(i, digit[i], color_digit[i]);
  }

  pDisplayDriver->sync();
}

int RandomColoredDigit::nextSubEffect() {
  srand(millis() % 0xFFFF);

  blinking_colon_color = generateRandomColor();
  color_digit[0] = generateRandomColor();
  color_digit[1] = generateRandomColor();
  color_digit[2] = generateRandomColor();
  color_digit[3] = generateRandomColor();

  return 0;
}
