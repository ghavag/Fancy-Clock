/*
* A AVR microcontroller based fancy clock
* Copyright (c) 2019 Alexander Graeb
*
* TODO: Which license to use?
*
* For more information see header file.
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

void FadingColors::update(unsigned long tnow, bool force_update) {
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
  update_blinking_colon(tnow);

  displayCurrentTime(color);

  pDisplayDriver->sync();
}

int FadingColors::nextSubEffect() {
  selected_sub_effect = (selected_sub_effect + 1) % 2;

  if (selected_sub_effect == 1) speed = 10;
  else speed = 1;

  return selected_sub_effect;
}
