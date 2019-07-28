/*
* A AVR microcontroller based fancy clock
* Copyright (c) 2019 Alexander Graeb
*
* TODO: Which license to use?
*
* This class implements a effect to display the time using multiple colors
* constantly altering by fading in and out. For more details see comments
* in BaseEffect.h.
*/

#ifndef FADING_COLORS_H_
#define FADING_COLORS_H_

#include "BaseEffect.h"

class FadingColors : public BaseEffect {
public:
  /*
  * Class constructor
  *
  * Params:
  *   DD: Pointer to the display driver class object
  */
  FadingColors(DisplayDriver *DD);

  void update(unsigned long tnow, bool force_update);

  int nextSubEffect();

private:
  cRGB color; // Color currently used to display the time

  /* For this effect the different sub-effects are just speed levels */
  uint8_t selected_sub_effect;

  uint8_t stage;
  uint8_t speed;
};

#endif //FADING_COLORS_H_
