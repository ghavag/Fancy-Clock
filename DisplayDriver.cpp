/*
 * This file is part of the project Fancy Clock
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

#include "DisplayDriver.h"
#include <stdlib.h>

DisplayDriver::DisplayDriver(const volatile uint8_t* port, volatile uint8_t* reg, uint8_t pin) : WS2812(LEDCount) {
  // Call some setup routines from the parent class / Light WS 2812 library
  setOutput(port, reg, pin);
  setColorOrderRGB();

  max_brightness = 0;

  init();
}

void DisplayDriver::init() {
  cRGB pixel;

  pixel.r = pixel.g = pixel.b = 0;

  for (int i = 0; i < LEDCount; i++) {
    set_crgb_at(i, pixel);
  }

  sync();
}

unsigned long long DisplayDriver::getDigitBitMap(uint8_t digit) {
  switch (digit) {
    case 0:
      return 0xACADCB2B6;
    case 1:
      return 0x16DB0000;
    case 2:
      return 0x4F5B02CB281;
    case 3:
      return 0x130ACB2CB200;
    case 4:
      return 0x520016FB05C0;
    case 5:
      return 0x890ACB016D82;
    case 6:
      return 0x310ACB0CB2B6;
    case 7:
      return 0x16DB6C00;
    case 8:
      return 0x730ACB2CB2CA;
    case 9:
      return 0x520ACADCB2C2;
    default:
      return 0;
  }
}

void DisplayDriver::setDigit(uint8_t index, uint8_t digit, uint8_t red, uint8_t green, uint8_t blue) {
  unsigned long long bit_map = getDigitBitMap(digit);
  cRGB value;

  for (int i = 0; i < LEDCountPerDigit; i++) {
    if (bit_map & ((unsigned long long) 1 << i)) {
      value.r = red;
      value.g = green;
      value.b = blue;
    } else {
      value.r = value.g = value.b = 0;
    }

    setLED(i + index * LEDCountPerDigit + 2, value);
  }
}

void DisplayDriver::setDigit(uint8_t index, uint8_t digit, cRGB color) {
  unsigned long long bit_map = getDigitBitMap(digit);
  cRGB value;

  for (int i = 0; i < LEDCountPerDigit; i++) {
    if (bit_map & ((unsigned long long) 1 << i)) {
      value = color;
    } else {
      value.r = value.g = value.b = 0;
    }

    setLED(i + index * LEDCountPerDigit + 2, value);
  }
}

uint8_t DisplayDriver::setLED(uint16_t index, cRGB px_value) {
  px_value.r *= (double)max_brightness / 255;
  px_value.g *= (double)max_brightness / 255;
  px_value.b *= (double)max_brightness / 255;

  set_crgb_at(index, px_value);
}

void DisplayDriver::sync() {
  WS2812::sync();
}
