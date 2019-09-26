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

#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "time.h"
#include "lib/uart/uart.h"
#include "lib/dcf77/DCF77.h"
#include "lib/ds1302/ds1302.h"
#include "DisplayDriver.h"

/* Include header files of all effects */
#include "effects/BaseEffect.h"
#include "effects/SimpleColor.h"
#include "effects/RandomColoredDigit.h"
#include "effects/FadingColors.h"

/*
* The typical main function
*/
int main(void);

/*
* Starts the main loop which loops forever
*/
void loop(void);

/*
* Converts a ds1302_struct into a BaseEffect::datetime struct
*
* Params:
*   *s: Pointer to the source struct of type tmElements_t
*   *d: Pointer to the destination struct of type ds1302_struct
*/
inline void convert_rtc2datetime(ds1302_struct *s, BaseEffect::datetime *d);

/*
* Converts a tmElements_t struct into a ds1302_struct struct
*
* Params:
*   *s: Pointer to the source struct of type tmElements_t
*   *d: Pointer to the destination struct of type ds1302_struct
*/
inline void convert_tmElements_t2rtc(tmElements_t *s, ds1302_struct *d);
