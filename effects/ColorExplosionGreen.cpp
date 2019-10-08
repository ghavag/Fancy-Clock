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

#include "ColorExplosionGreen.h"

#include <stdlib.h>

ColorExplosionGreen::ColorExplosionGreen(DisplayDriver *DD) : ColorExplosion(DD) {}

cRGB ColorExplosionGreen::generateRandomColor() {
  cRGB color;

  color.r = rand() % 64;
  color.g = 128 + rand() % 64;
  color.b = rand() % 64;

  return color;
}
