/*
 * This file is the main file of the project Fancy Clock
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

#include "main.h"

#define MAXIMUM_BRIGHTNESS 128 // Number between 0 and 255

#define BTN_NEXT_EFFECT 1
#define BTN_NEXT_SUB_EFFECT 2
#define BTN_DISPLAY_MODE 4

/*
* If the last time synchronization with DCF77 is longer ago as
* DCF77_MAX_SYNC_AGE milliseconds the colon start to blink again to signal that
* the time displayed could be wrong. Assuming the Arduino Nano board has a
* 16 MHz clock with a accuracy of 3000 ppm, we set this value to 1,250,000 to
* tolerate a time drift of less when plus/minus one minute.
*/
#define DCF77_MAX_SYNC_AGE 4294967295UL // 49.7103 days (biggest possible value)

/* Number of effets (always as much as effect header files are included) */
#define EFFECT_COUNT 3

DCF77 DCF = DCF77(0, 0);

/* Create and init library for DS1302 timekeeper support. */
DS1302 ds1302 = DS1302(
  &PORTC, &DDRC, PC2, // SCLK at Arduino port A2 (ATmega port C pin 2)
  &PORTC, &DDRC, &PINC, PC1, // IO at Arduino port A1 (ATmega port C pin 1)
  &PORTC, &DDRC, PC0 // CE at Arduino port A0 (ATmega port C pin 0)
);

/* Use port PC5 as output to send data to the display */
DisplayDriver DispDrv = DisplayDriver(&PORTD, &DDRD, PD4);

int main(void) {
  /*
  * Set port PB5 (Arduino port D13 = LED3) as output. This originally served
  * debugging purposes to make DCF77 flank changes visible. But it is a nice
  * feature to estimate whether the received DCF77 signal is good or bad so I
  * keeped it.
  */
  DDRB |= _BV(DDB5);
  PORTB ^= _BV(PB5);

  /* Init timer */
  time_init();

  /* UART initialization part */
  uart_init();
  stdout = &uart_output;
  stdin  = &uart_input;

  /*
  * Setup button inputs
  * Pins are inputs by default, enable only internal pull up resistors
  */
  PORTD |= _BV(PD7); // Arduino Nano pin D7 / Button: Switch display mode
  PORTB |= _BV(PB0); // Arduino Nano pin D8 / Button: Toggle time_ok
  PORTB |= _BV(PB1); // Arduino Nano pin D9 / Button: Next sub-effect
  PORTB |= _BV(PB2); // Arduino Nano pin D10 / Button: Next effect

  /* Setup potentiometer for brightness control */
  ADMUX = _BV(REFS0); // Use Vcc as reference voltage source
  ADMUX |= _BV(MUX1) | _BV(MUX0); // Select port PC3/ADC3 (A3 at Arduino Nano)
  ADCSRA |= _BV(ADEN); // Enable analog to digital converter (ADC)

  /* Init the DFC77 decoder library */
  PORTD |= _BV(PD2); // Enable internal pull up resistor
  EICRA |= _BV(ISC00); // Interrupt on any logical change of D2
  EIMSK |= _BV(INT0); // Enable interrupt INT0
  sei();

  loop(); // Call the main loop (function that never returns)

  return 0;
}

void loop() {
  unsigned char s, m, h;
  unsigned long DCFtime;
  unsigned long var_millis;
  unsigned long last_dcf77_update = 0;
  tmElements_t tm;
  ds1302_struct rtc;
  BaseEffect::datetime dt;
  uint8_t btn_pressed = 0;
  uint8_t btn_time_ok_press_time = 0;
  BaseEffect *effects[EFFECT_COUNT];
  uint8_t selected_effect = 0;
  bool time_ok = false;
  uint8_t display_mode = 0;
  uint8_t abrightness[3];
  uint8_t tmp, nbm = 0; // Number brightness measurements

  /* Instantiate all effets */
  SimpleColor eff_sc = SimpleColor(&DispDrv);
  effects[0] = &eff_sc;
  RandomColoredDigit eff_rcd = RandomColoredDigit(&DispDrv);
  effects[1] = &eff_rcd;
  FadingColors eff_fc = FadingColors(&DispDrv);
  effects[2] = &eff_fc;

  /* Dummy read of brightness potentiometer */
  ADCSRA |= _BV(ADSC);
  while(ADCSRA & _BV(ADSC));
  (void) ADCW;

  ADCSRA |= _BV(ADSC); // First "real" read of brightness potentiometer

  while(1) {
    var_millis = millis();

    /* Read the brightness potentiometer and apply the brightness */
    while(ADCSRA & _BV(ADSC));
    abrightness[nbm++] = (float)MAXIMUM_BRIGHTNESS/1023*ADCW;

    if (nbm >= sizeof(abrightness)/sizeof(uint8_t)) {
      for (uint8_t i = 0; i < (sizeof(abrightness)/sizeof(uint8_t) - 1); i++) {
        for (nbm = i + 1; nbm < sizeof(abrightness)/sizeof(uint8_t); nbm++) {
          if (abrightness[i] > abrightness[nbm]) {
            tmp = abrightness[i];
            abrightness[i] = abrightness[nbm];
            abrightness[nbm] = tmp;
          }
        }
      }

      DispDrv.max_brightness = abrightness[sizeof(abrightness)/sizeof(uint8_t)/2 + 1];
      nbm = 0;
    }

    ADCSRA |= _BV(ADSC); // Start the next measurement (to be read next cycle)

    ds1302.clock_burst_read((uint8_t *) &rtc);
    convert_rtc2datetime(&rtc, &dt);
    effects[selected_effect]->update(dt, time_ok, display_mode);

    /** Button handling **/
    /* Next effect */
    if (!(PINB & _BV(PB2)) && !(btn_pressed & BTN_NEXT_EFFECT)) {
      btn_pressed |= BTN_NEXT_EFFECT;
      selected_effect = (selected_effect + 1) % EFFECT_COUNT;
    } else if (PINB & _BV(PB2)) {
      btn_pressed &= (0xFF - BTN_NEXT_EFFECT);
    }
    /* Next sub-effect */
    if (!(PINB & _BV(PB1)) && !(btn_pressed & BTN_NEXT_SUB_EFFECT)) {
      btn_pressed |= BTN_NEXT_SUB_EFFECT;
      effects[selected_effect]->nextSubEffect();
    } else if (PINB & _BV(PB1)) {
      btn_pressed &= (0xFF - BTN_NEXT_SUB_EFFECT);
    }
    /* Display mode */
    if (!(PIND & _BV(PD7)) && !(btn_pressed & BTN_DISPLAY_MODE)) {
      btn_pressed |= BTN_DISPLAY_MODE;
      display_mode = (display_mode + 1) % DISPLAY_MODES;
    } else if (PIND & _BV(PD7)) {
      btn_pressed &= (0xFF - BTN_DISPLAY_MODE);
    }
    /* Time ok */
    if (!(PINB & _BV(PB0))) {
      if (btn_time_ok_press_time == 60) time_ok = !time_ok;
      if (btn_time_ok_press_time < 61) btn_time_ok_press_time++;
    } else {
      btn_time_ok_press_time = 0;
    }

    // Syncing clock with DCF77 if a new time is available
    if (DCF.getTimeAsStruct(&tm)) {
      printf("There is a new time!\n");

      convert_tmElements_t2rtc(&tm, &rtc);
      ds1302.clock_burst_write((uint8_t *) &rtc);

      time_ok = true;
      last_dcf77_update = var_millis;
    }
    // Let the colon blink again if the last sync was too long ago
    else if (var_millis - last_dcf77_update > DCF77_MAX_SYNC_AGE) {
      //effects[selected_effect]->setBlinkingColon(true);
      time_ok = false;
    }

    /*
    * Waiting for 34 milliseconds results in calling the update() method of the
    * effects ~29.4 times per second.
    */
    while (millis() - var_millis <= 34);
  }
}

inline void convert_rtc2datetime(ds1302_struct *s, BaseEffect::datetime *d) {
  /* Destination = Source */
  d->Seconds = s->Seconds;
  d->Seconds10 = s->Seconds10;
  d->Minutes = s->Minutes;
  d->Minutes10 = s->Minutes10;
  d->Hour = s->h24.Hour;
  d->Hour10 = s->h24.Hour10;

  d->Date = s->Date;
  d->Date10 = s->Date10;
  d->Month = s->Month;
  d->Month10 = s->Month10;
}

inline void convert_tmElements_t2rtc(tmElements_t *s, ds1302_struct *d) {
  /* Destination = Source */
  d->Seconds    = bin2bcd_l(s->Second);
  d->Seconds10  = bin2bcd_h(s->Second);
  d->CH         = 0; // 1 for Clock Halt, 0 to run;
  d->Minutes    = bin2bcd_l(s->Minute);
  d->Minutes10  = bin2bcd_h(s->Minute);
  // To use the 12 hour format,
  // use it like these four lines:
  //    d->h12.Hour   = bin2bcd_l( hours);
  //    d->h12.Hour10 = bin2bcd_h( hours);
  //    d->h12.AM_PM  = 0;     // AM = 0
  //    d->h12.hour_12_24 = 1; // 1 for 24 hour format
  d->h24.Hour   = bin2bcd_l(s->Hour);
  d->h24.Hour10 = bin2bcd_h(s->Hour);
  d->h24.hour_12_24 = 0; // 0 for 24 hour format
  d->Date       = bin2bcd_l(s->Day);
  d->Date10     = bin2bcd_h(s->Day);
  d->Month      = bin2bcd_l(s->Month);
  d->Month10    = bin2bcd_h(s->Month);
  d->Day        = 0;
  d->Year       = bin2bcd_l(s->Year + 1970 - 2000);
  d->Year10     = bin2bcd_h(s->Year + 1970 - 2000);
  d->WP = 0;
}

/*
 * This is part of the DCF77 library init
 * TODO: Should be done in DCF77 library directly
 */
ISR(INT0_vect) {
  PORTB ^= _BV(PB5);
  DCF.int0handler();
}
