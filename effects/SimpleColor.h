/*
* A AVR microcontroller based fancy clock
* Copyright (c) 2019 Alexander Graeb
*
* TODO: Which license to use?
*
* This class implements a simple effect display the time in one of various
* predefined colors. For more details see comments in BaseEffect.h.
*/

#ifndef SIMPLE_COLOR_H_
#define SIMPLE_COLOR_H_

#include "BaseEffect.h"
//#include "../DisplayDriver.h"

class SimpleColor : public BaseEffect {
public:
  /*
  * Class constructor
  *
  * Params:
  *   DD: Pointer to the display driver class object
  */
  SimpleColor(DisplayDriver *DD);

  void update(unsigned long tnow, bool force_update);

  int nextSubEffect();

private:
  cRGB color; // Color currently used to display the time
  uint8_t selected_sub_effect; // Index of the current selected sub-effect

  /*
  * Activates a sub-effect
  *
  * Params:
  *   sub_eff: Index of the sub-effect to be activated
  */
  void applySubEffect(uint8_t sub_eff);
};

#endif //SIMPLE_COLOR_H_
