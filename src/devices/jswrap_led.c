/*
 * jswrap_led.c
 *
 *  Created on: Mar 18, 2018
 *      Author: Eug
 * ----------------------------------------------------------------------------
 * This file is designed to be parsed during the build process
 *
 * JavaScript LED Functions
 * ----------------------------------------------------------------------------
 */
#include "jswrap_led.h"
#include "jsdevices.h"
#include "rdev_led.h"
#include "nrf_log.h"

/*JSON{
  "type" : "class",
  "class" : "LED"
}
This class provides RGB LED control
 */

/*JSON{
  "type" : "staticmethod",
  "class" : "LED",
  "name" : "set",
  "generate" : "jswrap_led_set",
  "params" : [
    ["data","int32","The data to set - color in RGB565 format"]
  ]
}
Set LED color to 'data' value
 */
void jswrap_led_set(int data) {
	NRF_LOG_DEBUG("LED set %x", data);
	RDevLedSetColor((LedColor)data);
	RDevLedSetIndication(LED_IND_EXTCOLOR);
}

/*JSON{
  "type" : "staticmethod",
  "class" : "LED",
  "name" : "off",
  "generate" : "jswrap_led_off"
}
Set LED color to 'data' value
 */
void jswrap_led_off(void) {
  RDevLedClearIndication(LED_IND_EXTCOLOR);
}
