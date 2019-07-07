/*
* A AVR microcontroller based fancy clock
* Copyright (c) 2019 Alexander Graeb
*
* TODO: Which license to use?
*
* This class implements a effect displaying each digit and the colon in a random
* generated color. The color changed every time the displayed digit changes its
* value.
*/

#ifndef RANDOM_COLORED_DIGIT_H_
#define RANDOM_COLORED_DIGIT_H_

#include "BaseEffect.h"

class RandomColoredDigit : public BaseEffect {
public:
  /*
  * Class constructor
  *
  * Params:
  *   DD: Pointer to the display driver class object
  */
  RandomColoredDigit(DisplayDriver *DD);

  void update(unsigned long tnow, bool force_update);

  /*
  * This effect doesn't implement sub-effects but instead generates completely
  * new colors if this method is called. Returns always 0.
  */
  int nextSubEffect();

private:
  /* Colors used for the digits. */
  cRGB color_digit[4];
};

#endif //RANDOM_COLORED_DIGIT_H_
