/*
 * Project Uhr2 - A fancy clock
 *
 * Copyright 2019 Alexander Graeb
 *
 */

#include <stdio.h>
#include <util/delay.h>

//#include "main.h"
#include "lib/uart/uart.h"

int main(void) {

    /* UART initialization part */
    uart_init();
    stdout = &uart_output;
    stdin  = &uart_input;

    char input;
    unsigned int i = 0;

    puts("Hello world!");

    // Call the main loop
    while(1) loop();

    return 0;
}

void loop() {
  static unsigned int i = 0;

  _delay_ms(1000);
  printf("Print %i\n", i++);
}
