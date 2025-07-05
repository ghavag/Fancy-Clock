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

#define DEBUG

#define MAXIMUM_BRIGHTNESS 128 // Number between 0 and 255

/*
* For the case the clock is running fast one second will be subtracted after
* each period of CORRECTOR_PERIOD seconds. For instance if the clock gains 10
* seconds per day, one second must be subtracted after each 8640 (60*60*24/10)
* seconds.
*/
#define CORRECTOR_PERIOD 14916

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
#define EFFECT_COUNT 9

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
  PORTD |= _BV(PD7); // Arduino Nano pin D7 / Button: Next effect
  PORTB |= _BV(PB0); // Arduino Nano pin D8 / Button: Next sub-effect
  PORTB |= _BV(PB1); // Arduino Nano pin D9 / Button: Switch display mode
  PORTB |= _BV(PB2); // Arduino Nano pin D10 / Button: Toggle time_ok

  /* Setup potentiometer for brightness control */
  ADMUX = _BV(REFS0); // Use Vcc as reference voltage source
  ADMUX |= _BV(MUX1) | _BV(MUX0); // Select port PC3/ADC3 (A3 at Arduino Nano)
  ADCSRA |= _BV(ADEN); // Enable analog to digital converter (ADC)

  /* Init the DFC77 decoder library */
  PORTD |= _BV(PD2); // Enable internal pull up resistor
  EICRA |= _BV(ISC00); // Interrupt on any logical change of D2
  EIMSK |= _BV(INT0); // Enable interrupt INT0
  sei();

  printf("\nI am Fancy Clock! Calling loop().\n");
  loop(); // Call the main loop (function that never returns)

  return 0;
}

void loop() {
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
  uint8_t brightness_last_read, brightness_diff_cnt = 0;
  int16_t brightness_last_stable = 0;
  unsigned long wait_turns = 0;
  uint8_t e = 0;
  bool night_mode = false;

  #ifdef CORRECTOR_PERIOD
  unsigned int cp_past_seconds = 0;
  int8_t cp_last_sec_value = -1;
  #endif

  #if AUTO_EFFECT_MODE_CNT
  uint8_t auto_effect_mode_playlist[AUTO_EFFECT_MODE_CNT];
  bool auto_effect_mode_trigger = false;
  uint8_t auto_effect_mode_cur_eff = 0;
  int num_sub_effects;
  #endif

  #ifdef DEBUG
  uint8_t debug_time_print_delay = 0;
  #endif

  /* Instantiate all effects and put them on the playlist for auto effect mode if desired. */
  const char add_pl_fmt[] = "Add effect %s (index %u) to autoeffect mode playlist.\n";

  // Simple Color
  SimpleColor eff_sc = SimpleColor(&DispDrv);
  effects[e] = &eff_sc;

  #if AUTO_EFFECT_SIMPLE_COLOR
    auto_effect_mode_playlist[selected_effect++] = e;
    printf(add_pl_fmt, "SimpleColor", e);
  #endif

  // Random Colored Digit
  RandomColoredDigit eff_rcd = RandomColoredDigit(&DispDrv);
  effects[++e] = &eff_rcd;

  #if AUTO_EFFECT_RANDOM_COLORED_DIGIT
    auto_effect_mode_playlist[selected_effect++] = e;
    printf(add_pl_fmt, "RandomColoredDigit", e);
  #endif

  // Fading Colors
  FadingColors eff_fc = FadingColors(&DispDrv);
  effects[++e] = &eff_fc;

  #if AUTO_EFFECT_FADING_COLORS
    auto_effect_mode_playlist[selected_effect++] = e;
    printf(add_pl_fmt, "FadingColors", e);
  #endif

  // Fading Digits
  FadingDigits eff_fd = FadingDigits(&DispDrv);
  effects[++e] = &eff_fd;

  #if AUTO_EFFECT_FADING_DIGITS
    auto_effect_mode_playlist[selected_effect++] = e;
    printf(add_pl_fmt, "FadingDigits", e);
  #endif

  // Color Explosion
  ColorExplosion eff_ce = ColorExplosion(&DispDrv);
  effects[++e] = &eff_ce;

  #if AUTO_EFFECT_COLOR_EXPLOSION
    auto_effect_mode_playlist[selected_effect++] = e;
    printf(add_pl_fmt, "ColorExplosion", e);
  #endif

  // Color Explosion Red
  ColorExplosionRed eff_cer = ColorExplosionRed(&DispDrv);
  effects[++e] = &eff_cer;

  #if AUTO_EFFECT_COLOR_EXPLOSION_RED
    auto_effect_mode_playlist[selected_effect++] = e;
    printf(add_pl_fmt, "ColorExplosionRed", e);
  #endif

  // Color Explosion Green
  ColorExplosionGreen eff_ceg = ColorExplosionGreen(&DispDrv);
  effects[++e] = &eff_ceg;

  #if AUTO_EFFECT_COLOR_EXPLOSION_GREEN
    auto_effect_mode_playlist[selected_effect++] = e;
    printf(add_pl_fmt, "ColorExplosionGreen", e);
  #endif

  // Color Explosion Blue
  ColorExplosionBlue eff_ceb = ColorExplosionBlue(&DispDrv);
  effects[++e] = &eff_ceb;

  #if AUTO_EFFECT_COLOR_EXPLOSION_BLUE
    auto_effect_mode_playlist[selected_effect++] = e;
    printf(add_pl_fmt, "ColorExplosionBlue", e);
  #endif

  // Dice Like Digits
  DiceLikeDigits eff_dld = DiceLikeDigits(&DispDrv);
  effects[++e] = &eff_dld;

  #if AUTO_EFFECT_DICE_LIKE_DIGITS
    auto_effect_mode_playlist[selected_effect++] = e;
    printf(add_pl_fmt, "DiceLikeDigits", e);
  #endif

  #if AUTO_EFFECT_MODE_CNT
  selected_effect = 0;
  #endif

  /*****************************************************************************************/

  // Initialize the random number generator
  ds1302.clock_burst_read((uint8_t *) &rtc);
  srand((uint16_t)(rtc.Seconds10 * 10 + rtc.Seconds + ((rtc.Minutes10 * 10 + rtc.Minutes) * 60) + ((rtc.h24.Hour10 * 10 + rtc.h24.Hour) * 3600)));

  night_mode = is_night(&rtc);

  #if AUTO_EFFECT_MODE_CNT
    #ifdef AUTO_EFFECT_MODE_RANDOMIZE
      auto_effect_mode_cur_eff = rand() % AUTO_EFFECT_MODE_CNT;
      selected_effect = auto_effect_mode_playlist[auto_effect_mode_cur_eff];
      effects[selected_effect]->select();
    #else
      selected_effect = 0; // Reset to 0 because be we diverted selected_effect above to fill the auto effect mode playlist
    #endif
  #endif

  /* Dummy read of brightness potentiometer */
  ADCSRA |= _BV(ADSC);
  while(ADCSRA & _BV(ADSC));
  (void) ADCW;

  ADCSRA |= _BV(ADSC); // First "real" read of brightness potentiometer

  printf("Entering main loop!\n");

  while(1) {
    var_millis = millis();

    /* Read the brightness potentiometer and collect a few results */
    while(ADCSRA & _BV(ADSC));
    brightness_last_read = (float)MAXIMUM_BRIGHTNESS/1023*ADCW;

    if ((brightness_last_read > (brightness_last_stable + 5)) && brightness_diff_cnt >= 0) {
      brightness_diff_cnt++;
    } else if (brightness_last_read < (brightness_last_stable - 5) && brightness_diff_cnt <= 0) {
      brightness_diff_cnt--;
    } else {
      brightness_diff_cnt = 0;
    }

    if (abs(brightness_diff_cnt) > 6) {
      brightness_diff_cnt = 0;
      brightness_last_stable = brightness_last_read;
      DispDrv.max_brightness = night_mode ? (brightness_last_read / 3) : brightness_last_read;
    }

    ADCSRA |= _BV(ADSC); // Start the next measurement (to be read next cycle)

    ds1302.clock_burst_read((uint8_t *) &rtc);
    convert_rtc2datetime(&rtc, &dt);
    effects[selected_effect]->update(dt, time_ok, display_mode);

    #ifdef CORRECTOR_PERIOD
      if (cp_last_sec_value < 0) {
        cp_last_sec_value = rtc.Seconds;
      } else if (cp_last_sec_value != rtc.Seconds) {
        cp_past_seconds++;
        cp_last_sec_value = rtc.Seconds;
      }

      /*
      * Note: We don't want to subtract the correction second if the current second is zero,
      * because we when would have to take care about the minutes, hours etc. as well.
      */
      if (rtc.Seconds10 != 0 && rtc.Seconds != 0 && cp_past_seconds >= CORRECTOR_PERIOD) {
        // Use cp_last_sec_value temporarily to do calculations
        cp_last_sec_value = bcd2bin(rtc.Seconds10, rtc.Seconds);
        cp_last_sec_value--;
        cp_past_seconds %= CORRECTOR_PERIOD;

        rtc.Seconds = bin2bcd_l(cp_last_sec_value);
        rtc.Seconds10 = bin2bcd_h(cp_last_sec_value);
        ds1302.clock_burst_write((uint8_t *) &rtc);

        cp_last_sec_value = rtc.Seconds;

        printf("Corrector second was applied.\n");
      }
    #endif

    #if AUTO_EFFECT_MODE_CNT
    if (rtc.Minutes10 == 0) {
      if (!auto_effect_mode_trigger) {
        night_mode = is_night(&rtc);

        #ifdef AUTO_EFFECT_MODE_RANDOMIZE
        auto_effect_mode_cur_eff = rand() % AUTO_EFFECT_MODE_CNT;
        #else
        auto_effect_mode_cur_eff = (auto_effect_mode_cur_eff + 1) % AUTO_EFFECT_MODE_CNT;
        #endif

        selected_effect = auto_effect_mode_playlist[auto_effect_mode_cur_eff];
        effects[selected_effect]->select();

        #ifdef DEBUG
        printf("Auto effect mode: Select effect no. %i\r", selected_effect);
        #endif

        // Also choose a random sub-effect
        num_sub_effects = effects[selected_effect]->getNumberOfSubEffects();

        if (num_sub_effects > 0) {
          effects[selected_effect]->applySubEffect(rand() % num_sub_effects);
        }
      }

      auto_effect_mode_trigger = true;
    } else {
      auto_effect_mode_trigger = false;
    }
    #endif

    /** Button handling **/
    /* Next effect */
    if (!(PIND & _BV(PD7)) && !(btn_pressed & BTN_NEXT_EFFECT)) {
      btn_pressed |= BTN_NEXT_EFFECT;
      selected_effect = (selected_effect + 1) % EFFECT_COUNT;
      effects[selected_effect]->select();
    } else if (PIND & _BV(PD7)) {
      btn_pressed &= (0xFF - BTN_NEXT_EFFECT);
    }
    /* Next sub-effect */
    if (!(PINB & _BV(PB0)) && !(btn_pressed & BTN_NEXT_SUB_EFFECT)) {
      btn_pressed |= BTN_NEXT_SUB_EFFECT;
      effects[selected_effect]->nextSubEffect();
    } else if (PINB & _BV(PB0)) {
      btn_pressed &= (0xFF - BTN_NEXT_SUB_EFFECT);
    }
    /* Display mode */
    if (!(PINB & _BV(PD1)) && !(btn_pressed & BTN_DISPLAY_MODE)) {
      btn_pressed |= BTN_DISPLAY_MODE;
      display_mode = (display_mode + 1) % DISPLAY_MODES;
    } else if (PINB & _BV(PD1)) {
      btn_pressed &= (0xFF - BTN_DISPLAY_MODE);
    }
    /* Time ok */
    if (!(PINB & _BV(PB2))) {
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

      #ifdef CORRECTOR_PERIOD
      cp_past_seconds = 0;
      cp_last_sec_value = 0;
      #endif
    }
    // Let the colon blink again if the last sync was too long ago
    else if (var_millis - last_dcf77_update > DCF77_MAX_SYNC_AGE) {
      //effects[selected_effect]->setBlinkingColon(true);
      time_ok = false;
    }

  #ifdef DEBUG
	if (debug_time_print_delay > 30) {
		debug_time_print_delay = 0;

		printf("Time: %d.%d.%d %02d:%02d:%02d\n",
      bcd2bin(rtc.Date10, rtc.Date), bcd2bin(rtc.Month10, rtc.Month), 2000 + bcd2bin( rtc.Year10, rtc.Year),
      bcd2bin(rtc.h24.Hour10, rtc.h24.Hour), bcd2bin(rtc.Minutes10, rtc.Minutes), bcd2bin(rtc.Seconds10, rtc.Seconds));
    printf("Wait turns: %lu\n", wait_turns/31);
    wait_turns = 0;
	} else {
		debug_time_print_delay++;
	}
  #endif

    /*
    * Waiting for 32 milliseconds results in calling the update() method of the
    * effects ~31 times per second.
    */
    while (millis() - var_millis < 32) wait_turns++;
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

bool is_night(ds1302_struct *t) {
  #ifdef NIGHT_MODE
    int h = bcd2bin(t->h24.Hour10, t->h24.Hour);
    bool ret = false;

    #if NIGHT_MODE_BEGIN_HOUR > NIGHT_MODE_END_HOUR
        ret = (h >= NIGHT_MODE_BEGIN_HOUR or h < NIGHT_MODE_END_HOUR);
    #else
        ret = (h >= NIGHT_MODE_BEGIN_HOUR and h < NIGHT_MODE_END_HOUR);
    #endif

    printf("is_night(): %s\n", (ret ? "true" : "false"));
  #endif // NIGHT_MODE

  return ret;
}

/*
 * This is part of the DCF77 library init
 * TODO: Should be done in DCF77 library directly
 */
ISR(INT0_vect) {
  PORTB ^= _BV(PB5);
  DCF.int0handler();
}
