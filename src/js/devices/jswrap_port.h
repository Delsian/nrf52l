/*
 * jswrap_port.h
 *
 *  Created on: 16.03.2018
 *      Author: Eug
 */

#ifndef JS_DEVICES_JSWRAP_PORT_H_
#define JS_DEVICES_JSWRAP_PORT_H_

#include "jsvar.h"

JsVar *jswrap_port_constructor();
void jswrap_port_set(JsVar *parent, JsVar *options);

#endif /* JS_DEVICES_JSWRAP_PORT_H_ */
