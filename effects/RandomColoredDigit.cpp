/*
* A AVR microcontroller based fancy clock
* Copyright (c) 2019 Alexander Graeb
*
* TODO: Which license to use?
*
* For more information see header file.
*/

#include "RandomColoredDigit.h"
#include <stdlib.h>

RandomColoredDigit::RandomColoredDigit(DisplayDriver *DD) : BaseEffect(DD) {
  nextSubEffect();
}

void RandomColoredDigit::update(unsigned long tnow, bool force_update) {
  update_blinking_colon(tnow);
  tmElements_t tm;
  uint8_t digit[4];
  static uint8_t old_digit[4];

  tm = getTimeOnly();
  digit[0] = tm.Minute % 10;
  digit[1] = tm.Minute / 10;
  digit[2] = tm.Hour % 10;
  digit[3] = tm.Hour / 10;

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
