/*
 * This file is part of the project Fancy Clock
 *
 * This effect uses random colors for each LED and, depending on the sub-effect,
 * refreshes the color fast, slow (once per second) or only on digit change. Be
 * warned if you are a epileptic.
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

#ifndef COLOR_EXPLOSION_H_
#define COLOR_EXPLOSION_H_

#include "BaseEffect.h"

class ColorExplosion : public BaseEffect {
public:
  /*
  * Class constructor
  *
  * Params:
  *   DD: Pointer to the display driver class object
  */
  ColorExplosion(DisplayDriver *DD);

  void update(datetime dt, bool time_is_synched, uint8_t dm);

  int nextSubEffect();

protected:
  cRGB color_upper, color_lower;

  uint8_t selected_sub_effect; // Index of the current selected sub-effect
  uint8_t delay; // Update only every delay update() method call

  /*
  * Activates a sub-effect
  *
  * Params:
  *   sub_eff: Index of the sub-effect to be activated
  */
  void applySubEffect(uint8_t sub_eff);

  void setDigit(uint8_t index, uint8_t digit);

  void update_blinking_colon();

  cRGB generateRandomColor();
};

#endif //COLOR_EXPLOSION_H_
