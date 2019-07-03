/*
* A AVR microcontroller based fancy clock
* Copyright (c) 2019 Alexander Graeb
*
* TODO: Which license to use?
*
* This file is part of the display driver implementation. The display consists
* of 194 WS2812 compatible LEDs (I use PL9823-F5 LEDs). Those LEDs can be
* controlled using only one single output pin at the microcontroller. To
* communicate with the LEDs, the Light WS2812 library is used.
*/

#ifndef DISPLAYDRIVER_H_
#define DISPLAYDRIVER_H_

#include "lib/light_ws2812/WS2812.h"

#define LEDCount 294
#define LEDCountPerDigit 48

class DisplayDriver : private WS2812 {
public:
  uint8_t max_brightness;

  /*
  * Class constructor
  */
  DisplayDriver(const volatile uint8_t* port, volatile uint8_t* reg, uint8_t pin);

  /*
  * Gets the bitmap that indicates which of the 48 LEDs of an digit segment to
  * be lightup in order to display the given digit.
  *
  * digit: Number from 0 to 9. Every value different from that returns a empty
  *         i. e. null bitmap.
  *
  * return: The bitmap
  */
  unsigned long long getDigitBitMap(uint8_t digit);

  /*
   * Sets a number (a single digit) on the hardware digit display.
   *
   * Params:
   *   index: The index of the hardware digit display starting from 0 (the most right one).
   *   digit: The digit (0 to 9) to be displayed.
   *   red, green, blue: The intensity of each color (between 0 and 255).
   */
  void setDigit(uint8_t index, uint8_t digit, uint8_t red, uint8_t green, uint8_t blue);

  /*
   * Sets a number (a single digit) on the hardware digit display.
   *
   * Params:
   *   index: The index of the hardware digit display starting from 0 (the most right one).
   *   digit: The digit (0 to 9) to be displayed.
   *   color: The intensity of each color as cRGB struct.
   */
  void setDigit(uint8_t index, uint8_t digit, cRGB color);

  /*
  * Sets the RGB value of an LED
  *
  * Params:
  *   index: Index of the LED at which 0 addresses the first LED
  *   The RGB value to assign to the LED
  */
  uint8_t setLED(uint16_t index, cRGB px_value);

  /*
  * Writes the data to the LEDs
  */
  void sync();

private:
  /*
  * Does some initialization work for the display
  */
  void init();
};

#endif //DISPLAYDRIVER_H_
