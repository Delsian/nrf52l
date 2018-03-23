/*
 * jswrap_range.h
 *
 *  Created on: Mar 23, 2018
 *      Author: Eug
 */

#ifndef DEVICES_JSWRAP_RANGE_H_
#define DEVICES_JSWRAP_RANGE_H_

JsVar *jswrap_range_constructor(int port);
JsVarInt jswrap_range_read(JsVar *parent);

#endif /* DEVICES_JSWRAP_RANGE_H_ */
