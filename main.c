/*
 * Project Uhr2 - A fancy clock
 *
 * Copyright 2019 Alexander Graeb
 *
 */

#include <stdio.h>
#include <util/delay.h>

#include "main.h"
#include "time.h"
#include "lib/uart/uart.h"

int main(void) {
  unsigned char s, m, h;

  /* Init timer */
  time_init();

  /* UART initialization part */
  uart_init();
  stdout = &uart_output;
  stdin  = &uart_input;

  puts("Please set the time!");

  printf("Hours: ");
  scanf("%hhu", &h);

  printf("\nMinutes: ");
  scanf("%hhu", &m);

  printf("\nSeconds: ");
  scanf("%hhu", &s);

  printf("\n");

  set_time(h, m, s);

  // Call the main loop
  while(1) loop();

  return 0;
}

void loop() {
  unsigned char s, m, h;

  _delay_ms(1000);
  get_time(&h, &m, &s);
  printf("%02u:%02u:%02u\n", h, m, s);
}
