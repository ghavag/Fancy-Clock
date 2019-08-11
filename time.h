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

#ifndef TIME_H_
#define TIME_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY  (SECS_PER_HOUR * 24UL)

typedef struct  {
  uint8_t Second;
  uint8_t Minute;
  uint8_t Hour;
  uint8_t Wday;   // day of week, sunday is day 1
  uint8_t Day;
  uint8_t Month;
  uint8_t Year;   // offset from 1970;
} tmElements_t;

void time_init(void);
unsigned long millis();
unsigned long makeTime(tmElements_t &tm);
unsigned long now();
void setTime(unsigned long t);
//tmElements_t timeFromStamp(unsigned long timeInput);
tmElements_t getTimeOnly();
//tmElements_t getTimeOnly(unsigned long now_ms);

#ifdef __cplusplus
}
#endif

#endif // TIME_H_
