/*
 * jswrap_led.c
 *
 *  Created on: Mar 18, 2018
 *      Author: Eug
 * ----------------------------------------------------------------------------
 * This file is designed to be parsed during the build process
 *
 * JavaScript OneWire Functions
 * ----------------------------------------------------------------------------
 */
#include "jswrap_led.h"
#include "jsdevices.h"
#include "rdev_led.h"

/*JSON{
  "type" : "class",
  "class" : "LED"
}
This class provides RGB LED control
 */

/*JSON{
  "type" : "constructor",
  "class" : "LED",
  "name" : "LED",
  "generate" : "jswrap_led_constructor"
}
 */
JsVar *jswrap_led_constructor() {
  return jsvNewObject();
}

/*JSON{
  "type" : "method",
  "class" : "LED",
  "name" : "set",
  "generate" : "jswrap_led_set",
  "params" : [
    ["data","int32","The data to set - color in RGB565 format"]
  ]
}
Set LED color to 'data' value
 */
void jswrap_led_set(JsVar *parent, int data) {
  NOT_USED(parent);
  RDevLedSetColor((LedColor)data);
  RDevLedSetIndication(LED_IND_EXTCOLOR);
}
