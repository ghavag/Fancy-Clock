/*
 * Project Uhr2 - A fancy clock
 *
 * Copyright 2019 Alexander Graeb
 *
 */

#define MAXIMUM_BRIGHTNESS 128 // Number between 0 and 255

#define BTN_NEXT_SUB_EFFECT 1

#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "main.h"
#include "time.h"
#include "lib/uart/uart.h"
#include "lib/dcf77/DCF77.h"
#include "DisplayDriver.h"
//#include "effects/BaseEffect.h"
#include "effects/SimpleColor.h"

DCF77 DCF = DCF77(0, 0);

// Use Port PC5 as output so send data to the display
DisplayDriver DispDrv = DisplayDriver(&PORTC, &DDRC, PC5);
//BaseEffect BE = BaseEffect(&DispDrv);
SimpleColor BE =  SimpleColor(&DispDrv);

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

  /* Setup button inputs */
  PORTD |= _BV(PD4); // Enable internal pull up resistors

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
  tmElements_t tm;
  uint8_t btn_pressed = 0;

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

    BE.update(var_millis, false);

    /* Button handling */
    if (!(PIND & _BV(PD4)) && !(btn_pressed & BTN_NEXT_SUB_EFFECT)) {
      btn_pressed |= BTN_NEXT_SUB_EFFECT;
      BE.nextSubEffect();
    } else if (PIND & _BV(PD4)) {
      btn_pressed &= (0xFF - BTN_NEXT_SUB_EFFECT);
    }

    // Syncing time with DCF77 if a new time is available
    if (DCFtime = DCF.getTime()) {
      printf("There is a new time!\n");
      setTime(DCFtime);
      BE.setBlinkingColon(false);
      //printf("The new time is: %02u:%02u:%02u\n", tm.Hour, tm.Minute, tm.Second);
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
