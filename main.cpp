/*
 * Project Uhr2 - A fancy clock
 *
 * Copyright 2019 Alexander Graeb
 *
 */

#define MAXIMUM_BRIGHTNESS 4 // Number between 0 and 256

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

  /* Init the DFC77 decoder library */
  PORTD |= _BV(PD2); // Enable internal pull up resistor
  EICRA |= _BV(ISC00); // Interrupt on any logical change of D2
  EIMSK |= _BV(INT0); // Enable interrupt INT0
  sei();

  DispDrv.max_brightness = MAXIMUM_BRIGHTNESS;

  // Call the main loop
  loop();

  return 0;
}

void loop() {
  unsigned char s, m, h;
  unsigned long DCFtime;
  unsigned long var_millis;
  tmElements_t tm;
  uint8_t btn_pressed = 0;

  while(1) {
    //get_time(&h, &m, &s);
    //tm = getTimeOnly();
    //printf("%02u:%02u:%02u\n", tm.Hour, tm.Minute, tm.Second);
    var_millis = millis();

    BE.update(var_millis, false);

    /* Button handling */
    if (!(PIND & _BV(PD4)) && !(btn_pressed & BTN_NEXT_SUB_EFFECT)) {
      btn_pressed |= BTN_NEXT_SUB_EFFECT;
      BE.nextSubEffect();
    } else if (PIND & _BV(PD4)) {
      btn_pressed &= (0xFF - BTN_NEXT_SUB_EFFECT);
    }

    /*
    * Waiting for 34 milliseconds results in calling the update() method of the
    * effects ~29.4 times per second.
    * It is important to run this delay loop before syncing the time with DCF77,
    * otherwise we could get stuck in the delay loop for a very long time.
    */
    while (millis() - var_millis > 34);

    // Syncing time with DCF77 if a new time is available
    if (DCFtime = DCF.getTime()) {
      printf("There is a new time!\n");
      setTime(DCFtime);
      BE.setBlinkingColon(false);
      //printf("The new time is: %02u:%02u:%02u\n", tm.Hour, tm.Minute, tm.Second);
    }
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
