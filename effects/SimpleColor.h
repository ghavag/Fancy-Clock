/*
 * This file is part of the project Fancy Clock
 *
 * This effect does nothing else than displaying the time in different colors,
 * each implemented as a sub-effect.
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

  void update(unsigned long tnow, bool time_is_synched, uint8_t dm);

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
