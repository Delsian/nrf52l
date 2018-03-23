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
int jswrap_getport(JsVar *parent);

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
