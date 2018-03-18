/*
 * jswrap_led.h
 *
 *  Created on: Mar 18, 2018
 *      Author: Eug
 */

#ifndef DEVICES_JSWRAP_LED_H_
#define DEVICES_JSWRAP_LED_H_
#include "jsvar.h"

// Two includes for wrapper only
#include "jsparse.h"
#include "jspin.h"

JsVar *jswrap_led_constructor();
void jswrap_led_set(JsVar *parent, int data);

#endif /* DEVICES_JSWRAP_LED_H_ */
