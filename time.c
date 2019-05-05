#include <avr/interrupt.h>

volatile unsigned long milliseconds = 0;

volatile unsigned char seconds = 0;
volatile unsigned char minutes = 0;
volatile unsigned char hours = 0;

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

  // Debug code
  //DDRB |= _BV(DDB5); // Port PB5 als Ausgang verwenden.
}

ISR (TIMER0_COMPA_vect)
{
	//PORTB ^= _BV(PB5);
  /*
   * Comment and idea borrowed from Arduino project:
   * copy these to local variables so they can be stored in registers
   * (volatile variables must be read from memory on every access)
   */
  unsigned long ms = milliseconds;

  ms += 16;
  milliseconds = ms;

  if ((ms % 1000) == 8 || (ms % 1000) == 992) {
    unsigned char s = seconds;
    s = (s + 1) % 60;
    seconds = s;
    if (s == 0) {
      unsigned char min = minutes;
      min = (min + 1) % 60;
      minutes = min;
  		if (min == 0) {
        unsigned char h = hours;
        h = (h + 1) % 24;
        hours = h;
  		}
  	}
  }
}

void get_time(unsigned char *h, unsigned char *m, unsigned char *s) {
  uint8_t oldSREG = SREG;

  /*
   * Disable interrupts while we read or we might get inconsistent
   * values (e.g. in the middle of a write)
   */
  cli();

  *h = hours;
  *m = minutes;
  *s = seconds;

  SREG = oldSREG;
}

void set_time(unsigned char h, unsigned char m, unsigned char s) {
  uint8_t oldSREG = SREG;

  cli();

  hours = h;
  minutes = m;
  seconds = s;

  SREG = oldSREG;
}

unsigned long millis() {
  uint8_t oldSREG = SREG;
  cli();

  unsigned long ms = milliseconds;

  SREG = oldSREG;
  return ms;
}
