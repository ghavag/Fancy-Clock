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

#ifndef CONFIG_H_
#define CONFIG_H_

/*
* The auto effect mode atomatically chooses a new effect from a given playlist every
* hour if activated. Which effects are on the playlist depends on whether the below
* C macros are defined to 0 or 1. All macros defined to be 0 automatically disables
* the auto effect mode. 
*/

// The value must be either 0 (effect NOT on the playlist) or 1 (effect is on the playlist)
#define AUTO_EFFECT_SIMPLE_COLOR 1
#define AUTO_EFFECT_RANDOM_COLORED_DIGIT 1
#define AUTO_EFFECT_FADING_COLORS 1
#define AUTO_EFFECT_FADING_DIGITS 1

#define AUTO_EFFECT_COLOR_EXPLOSION 0
#define AUTO_EFFECT_COLOR_EXPLOSION_RED 0
#define AUTO_EFFECT_COLOR_EXPLOSION_GREEN 0
#define AUTO_EFFECT_COLOR_EXPLOSION_BLUE 0

#define AUTO_EFFECT_DICE_LIKE_DIGITS 1

// If defined the next effect will we choosen randomly
#define AUTO_EFFECT_MODE_RANDOMIZE

/*
* PART BELOW NOT MEANT TO BE CHANGED
*/

/* Calculate number of effects on the playlist */
#define AUTO_EFFECT_MODE_CNT (AUTO_EFFECT_SIMPLE_COLOR + AUTO_EFFECT_RANDOM_COLORED_DIGIT + AUTO_EFFECT_FADING_COLORS + AUTO_EFFECT_FADING_DIGITS \
                            + AUTO_EFFECT_COLOR_EXPLOSION + AUTO_EFFECT_COLOR_EXPLOSION_RED + AUTO_EFFECT_COLOR_EXPLOSION_GREEN + AUTO_EFFECT_COLOR_EXPLOSION_BLUE \
                            + AUTO_EFFECT_DICE_LIKE_DIGITS)

#endif // CONFIG_H_
