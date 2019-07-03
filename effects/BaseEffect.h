/*
* A AVR microcontroller based fancy clock
* Copyright (c) 2019 Alexander Graeb
*
* TODO: Which license to use?
*
* This time is displayed using various effets. Each effect is implemented in its
* own class. Every such class needs to inherit from this BaseEffect class.
*/

#ifndef BASE_EFFECT_H_
#define BASE_EFFECT_H_

#include "../time.h"
#include "../DisplayDriver.h"

/*
* Seems that AVR-GCC does not correctly implement pure virtual functions (see
* https://www.avrfreaks.net/comment/341297#comment-341297). The implementation
* is located in BaseEffect.cpp.
*/
extern "C" void __cxa_pure_virtual(void);

class BaseEffect {
public:
  /*
  * If the colon is set to blink, blinking_colon_hpd is the half period
  * duration (hpd) in milliseconds. If set to 500 for example, the colon lights
  * up for 500 ms, then turns off for another 500 ms and so on.
  */
  unsigned int blinking_colon_hpd;

  /*
  * cRGB value representing the color in which the colon blinks.
  */
  cRGB blinking_colon_color;

  /*
  * Class constructor
  *
  * Params:
  *   DD: Pointer to the display driver class object
  */
  BaseEffect(DisplayDriver *DD);

  /*
  * This method should be called regulary (i. e. every few milliseconds). It
  * will update the display according to the effects behaviour.
  *
  * tnow: The current time in milliseconds.
  * max_brightness: The current maximum brightness. Some effentcs may use that
  *                  information to determine whether to update the display on
  *                  change of the maximum brightness.
  */
  void update(unsigned long tnow, bool force_update);

  /*
  * Turn colon blinking on or off.
  *
  * blink: True lets the colon blink, false turn colon on constantly.
  */
  void setBlinkingColon(bool blink);

  /*
  * Effects may implement some sub-effects. If this method is called, the
  * effect shall switch to the next sub-effect. If the last sub-effect is
  * selected, the first sub-effect shall be selected again.
  *
  * return: The number of the selected sub-effect starting with zero.
  */
  virtual int nextSubEffect() = 0;

protected:
  /* Pointer to the display driver */
  DisplayDriver *pDisplayDriver;

  /*
  * A blinking colon indicates that the time is not synced to DCF77 (anymore).
  * Each effect should let the colon blink if this variable is set to true.
  */
  bool blinking_colon;

  /*
  * This method is usually called from the update() method if blinking_colon is
  * true. This method lets the colon blink.
  *
  * tnow: The current time in milliseconds.
  */
  void update_blinking_colon(unsigned long tnow);

  /*
  * Set the display to the current time.
  *
  * color: Color in which the time should be displayed.
  */
  void displayCurrentTime(cRGB color);
};

#endif //BASE_EFFECT_H_
