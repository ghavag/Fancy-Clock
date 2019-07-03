/*
* A AVR microcontroller based fancy clock
* Copyright (c) 2019 Alexander Graeb
*
* TODO: Which license to use?
*
* For more information see header file.
*/

#include "SimpleColor.h"

#define EFFECT_COUNT 7

SimpleColor::SimpleColor(DisplayDriver *DD) : BaseEffect(DD) {
  selected_sub_effect = 0;
  applySubEffect(selected_sub_effect);
}

void SimpleColor::update(unsigned long tnow, bool force_update) {
  update_blinking_colon(tnow);

  displayCurrentTime(color);

  pDisplayDriver->sync();
}

int SimpleColor::nextSubEffect() {
  selected_sub_effect = (selected_sub_effect + 1) % EFFECT_COUNT;
  applySubEffect(selected_sub_effect);

  return selected_sub_effect;
}

void SimpleColor::applySubEffect(uint8_t sub_eff) {
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
