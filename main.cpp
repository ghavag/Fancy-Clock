/*
 * Project Uhr2 - A fancy clock
 *
 * Copyright 2019 Alexander Graeb
 *
 */

#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "main.h"
#include "time.h"
#include "lib/uart/uart.h"
#include "lib/dcf77/DCF77.h"

DCF77 DCF = DCF77(0, 0);

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

  /* Init the DFC77 decoder library */
  //DDRD |= _BV(DDD2); // This is pin D2 on Arduino Nano as well
  PORTD |= _BV(PORTD2); // Enable internal pull up resistor
  EICRA |= _BV(ISC00); // Interrupt on any logical change of D2
  EIMSK |= _BV(INT0); // Enable interrupt INT0
  sei();

  /*puts("Please set the time!");

  printf("Hours: ");
  scanf("%hhu", &h);

  printf("\nMinutes: ");
  scanf("%hhu", &m);

  printf("\nSeconds: ");
  scanf("%hhu", &s);

  printf("\n");

  set_time(h, m, s);*/
  set_time(0, 0, 0);

  // Call the main loop
  while(1) loop();

  return 0;
}

void loop() {
  unsigned char s, m, h;

  _delay_ms(1000);
  get_time(&h, &m, &s);
  printf("%02u:%02u:%02u\n", h, m, s);

  if (DCF.getTime()) printf("There is a new time!\n");
}

ISR(INT0_vect) {
  PORTB ^= _BV(PB5);
  DCF.int0handler();
}
