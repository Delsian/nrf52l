/*
 * jswrap_range.c
 *
 *  Created on: Mar 22, 2018
 *      Author: Eug
 * ----------------------------------------------------------------------------
 * This file is designed to be parsed during the build process
 *
 * JavaScript Motor Functions
 * ----------------------------------------------------------------------------
 */
#include "jsdevices.h"
#include "r0b1c_device.h"
#include "rdev_proto.h"
#include "nrf_log.h"
#include "jsparse.h"
#include "rdev_proto.h"
#include "jsinteractive.h"
int jswrap_getport(JsVar *parent);
static JsVar* gtRO;

#define DATA_CALLBACK_NAME JS_EVENT_PREFIX"data"
#define OFFRANGE_CALLBACK_NAME JS_EVENT_PREFIX"off_range"

/*JSON{
  "type" : "class",
  "class" : "Range"
}
This class provides Range sensor access
 */

/*JSON{
  "type" : "constructor",
  "class" : "Range",
  "name" : "Range",
  "generate" : "jswrap_range_constructor",
  "params" : [
    ["port","int","The port sensor connected to"]
  ],
  "return" : ["JsVar","A Range object"]
}

 */
JsVar *jswrap_range_constructor(int port) {
  JsVar *ro = jspNewObject(0, "Range");
  if (ro && RDeviceChange(port, RDEV_ULTRASONIC) == RDERR_OK) { // ToDo motor size
	  jsvObjectSetChildAndUnLock(ro, "port", jsvNewFromInteger(port));
	  RDevRangeScanEnable(true);
	  gtRO = ro;
	  return ro;
  }
  return 0;
}

/*JSON{
  "type" : "event",
  "class" : "Range",
  "name" : "data",
  "params" : [
    ["data","int","Result of last measurement"]
  ]
}
The `data` event is called when data is received. If a handler is defined with `X.on('data', function(data) { ... })` then it will be called.
 */

/*JSON{
  "type" : "event",
  "class" : "Range",
  "name" : "off_range"
}
The `off_range` event is called when no obstacles in range after range reported
 */

/*JSON{
  "type" : "method",
  "class" : "Range",
  "name" : "read",
  "generate" : "jswrap_range_read",
  "return" : ["int","Last measured value"]
}
Return last measured value or 0xFF for "out of range"
 */
JsVarInt jswrap_range_read(JsVar *parent) {
	if (!jsvIsObject(parent)) return 0;
	int port = jswrap_getport(parent);
    return (JsVarInt)RDevRangeGet(port);
}

// === Event emitter ToDo
void jswrap_range_emit(uint8_t range)
{
	if (gtRO) {
		JsVar *callback = jsvObjectGetChild(gtRO, OFFRANGE_CALLBACK_NAME, 0);
		if(range < 0xFF) {
			callback = jsvObjectGetChild(gtRO, DATA_CALLBACK_NAME, 0);
		}
		if (callback) {
			JsVar *r = jsvNewFromInteger(range);
		    jsiExecuteEventCallback(0, callback, 1, &r);
		    jsvUnLock(callback);
		    jsvUnLock(r);
		}
	}
}
