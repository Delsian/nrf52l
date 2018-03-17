/*
 * jswrap_port.c
 *
 *  Created on: 16.03.2018
 *      Author: Eug
 */

#include "jsdevices.h"
#include "jsinteractive.h"
#include "nrf_log.h"

JsVar *jswrap_port_constructor() {
  return jsvNewObject();
}

void jswrap_port_set(JsVar *parent, JsVar *options)
{
	NRF_LOG_DEBUG("port");
}
