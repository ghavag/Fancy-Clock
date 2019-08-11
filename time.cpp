/*
 * This file is part of the project Fancy Clock
 *
 * This file implements functionality for handling the time.
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

#include "time.h"
#include <avr/interrupt.h>

volatile unsigned long milliseconds = 0;

static uint32_t prevMillis = 0;
static uint32_t sysTime = 0;

void time_init() {
  /*
   * Init Timer0: With this setup a interrupt routine
   * will be called every 16 ms.
   */
  TCCR0A |= _BV(WGM01); // Activate CTC mode
  TCCR0B |= _BV(CS02) + _BV(CS00); // Set prescaler to 1024
  OCR0A = 249; // Interrupt if counter reaches 249.
  TIMSK0 |= _BV(OCIE0A); // Activate interrupt for CTC mode
  GTCCR |= _BV(TSM); // Activate timer
  sei();
}

ISR (TIMER0_COMPA_vect)
{
  milliseconds += 16;
}

unsigned long millis() {
  uint8_t oldSREG = SREG;
  cli();

  unsigned long ms = milliseconds;

  SREG = oldSREG;
  return ms;
}

// leap year calulator expects year argument as years offset from 1970
#define LEAP_YEAR(Y)     ( ((1970+Y)>0) && !((1970+Y)%4) && ( ((1970+Y)%100) || !((1970+Y)%400) ) )

static const uint8_t monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31}; // API starts months from 1, this array starts from 0

unsigned long makeTime(tmElements_t &tm) {
  int i;
  uint32_t seconds;

  // seconds from 1970 till 1 jan 00:00:00 of the given year
  seconds = tm.Year*(SECS_PER_DAY * 365);
  for (i = 0; i < tm.Year; i++) {
    if (LEAP_YEAR(i)) {
      seconds +=  SECS_PER_DAY;   // add extra days for leap years
    }
  }

  // add days for this year, months start from 1
  for (i = 1; i < tm.Month; i++) {
    if ( (i == 2) && LEAP_YEAR(tm.Year)) {
      seconds += SECS_PER_DAY * 29;
    } else {
      seconds += SECS_PER_DAY * monthDays[i-1];  //monthDay array starts from 0
    }
  }
  seconds += (tm.Day-1) * SECS_PER_DAY;
  seconds += tm.Hour * SECS_PER_HOUR;
  seconds += tm.Minute * SECS_PER_MIN;
  seconds += tm.Second;
  return (unsigned long)seconds;
}

unsigned long now() {
  // calculate number of seconds passed since last call to now()
  while (millis() - prevMillis >= 1000) {
    // millis() and prevMillis are both unsigned ints thus the subtraction will always be the absolute value of the difference
    sysTime++;
    prevMillis += 1000;
  }

  return (unsigned long)sysTime;
}

void setTime(unsigned long t) {
  sysTime = (uint32_t)t;
  //Status = timeSet;
  prevMillis = millis();  // restart counting from now (thanks to Korman for this fix)
}

/*tmElements_t timeFromStamp(unsigned long timeInput) {
  // break the given time_t into time components
  // this is a more compact version of the C library localtime function
  // note that year is offset from 1970 !!!
  tmElements_t tm;
  uint8_t year;
  uint8_t month, monthLength;
  uint32_t time;
  unsigned long days;

  time = (uint32_t)timeInput;
  tm.Second = time % 60;
  time /= 60; // now it is minutes
  tm.Minute = time % 60;
  time /= 60; // now it is hours
  tm.Hour = time % 24;
  time /= 24; // now it is days
  tm.Wday = ((time + 4) % 7) + 1;  // Sunday is day 1

  year = 0;
  days = 0;
  while((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= time) {
    year++;
  }
  tm.Year = year; // year is offset from 1970

  days -= LEAP_YEAR(year) ? 366 : 365;
  time  -= days; // now it is days in this year, starting at 0

  days=0;
  month=0;
  monthLength=0;
  for (month=0; month<12; month++) {
    if (month==1) { // february
      if (LEAP_YEAR(year)) {
        monthLength=29;
      } else {
        monthLength=28;
      }
    } else {
      monthLength = monthDays[month];
    }

    if (time >= monthLength) {
      time -= monthLength;
    } else {
        break;
    }
  }
  tm.Month = month + 1;  // jan is month 1
  tm.Day = time + 1;     // day of month

  return tm;
}*/

tmElements_t getTimeOnly() {
  tmElements_t tm;
  uint8_t year;
  uint8_t month, monthLength;
  uint32_t time;
  unsigned long days;

  time = (uint32_t)now();
  tm.Second = time % 60;
  time /= 60; // now it is minutes
  tm.Minute = time % 60;
  time /= 60; // now it is hours
  tm.Hour = time % 24;
  time /= 24; // now it is days

  return tm;
}

/*tmElements_t getTimeOnly(unsigned long now_ms) {
  tmElements_t tm;
  uint8_t year;
  uint8_t month, monthLength;
  uint32_t time;
  unsigned long days;

  // calculate number of seconds passed since last call to now()
  while (now_ms - prevMillis >= 1000) {
    // millis() and prevMillis are both unsigned ints thus the subtraction will always be the absolute value of the difference
    sysTime++;
    prevMillis += 1000;
  }

  time = (unsigned long)sysTime;
  tm.Second = time % 60;
  time /= 60; // now it is minutes
  tm.Minute = time % 60;
  time /= 60; // now it is hours
  tm.Hour = time % 24;
  time /= 24; // now it is days

  return tm;
}*/
