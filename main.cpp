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
#define DCF77_MAX_SYNC_AGE 1250000UL

#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "main.h"
#include "time.h"
#include "lib/uart/uart.h"
#include "lib/dcf77/DCF77.h"
#include "DisplayDriver.h"

/* Include header files of all effects */
#include "effects/SimpleColor.h"
#include "effects/RandomColoredDigit.h"
#include "effects/FadingColors.h"

/* Number of effets (always as much as effect header files are included) */
#define EFFECT_COUNT 3

DCF77 DCF = DCF77(0, 0);

/* Use port PC5 as output to send data to the display */
DisplayDriver DispDrv = DisplayDriver(&PORTC, &DDRC, PC5);

int main(void) {
  unsigned char s, m, h;

  // Debug code
  DDRB |= _BV(DDB5); // Port PB5 als Ausgang verwenden.
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
  PORTB |= _BV(PB2);
  PORTB |= _BV(PD1);
  PORTD |= _BV(PD7);

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
  uint8_t btn_pressed = 0;
  BaseEffect *effects[EFFECT_COUNT];
  uint8_t selected_effect = 0;
  bool dcf_synced = false;
  uint8_t display_mode = 0;

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
    DispDrv.max_brightness = (float)MAXIMUM_BRIGHTNESS/1023*ADCW;
    ADCSRA |= _BV(ADSC); // Start the next measurement (to be read next cycle)

    effects[selected_effect]->update(var_millis, dcf_synced, display_mode);

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

    // Syncing clock with DCF77 if a new time is available
    if (DCFtime = DCF.getTime()) {
      printf("There is a new time!\n");
      setTime(DCFtime);
      //effects[selected_effect]->setBlinkingColon(false);
      dcf_synced = true;
      last_dcf77_update = var_millis;
    }
    // Let the colon blink again if the last sync was too long ago
    else if (var_millis - last_dcf77_update > DCF77_MAX_SYNC_AGE) {
      //effects[selected_effect]->setBlinkingColon(true);
      dcf_synced = false;
    }

    /*
    * Waiting for 34 milliseconds results in calling the update() method of the
    * effects ~29.4 times per second.
    */
    while (millis() - var_millis <= 34);
  }
}

/*
 * This is part of the DCF77 library init
 * TODO: Should be done in DCF77 library directly
 */
ISR(INT0_vect) {
  PORTB ^= _BV(PB5);
  DCF.int0handler();
}
