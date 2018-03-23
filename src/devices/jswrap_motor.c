/*
 * jswrap_motor.c
 *
 *  Created on: Mar 19, 2018
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
int jswrap_getport(JsVar *parent);

/*JSON{
  "type" : "class",
  "class" : "Motor"
}
This class provides Motor control
 */

/*JSON{
  "type" : "constructor",
  "class" : "Motor",
  "name" : "Motor",
  "generate" : "jswrap_motor_constructor",
  "params" : [
    ["port","int","The port motor connected to"]
  ],
  "return" : ["JsVar","A Motor object"]
}

 */
JsVar *jswrap_motor_constructor(int port) {
  JsVar *mo = jspNewObject(0, "Motor");
  if (mo && RDeviceChange(port, RDEV_MOTOR_M) == RDERR_OK) { // ToDo motor size
	  jsvObjectSetChildAndUnLock(mo, "port", jsvNewFromInteger(port));
	  return mo;
  }
  return 0;
}

/*JSON{
  "type" : "method",
  "class" : "Motor",
  "name" : "write",
  "generate" : "jswrap_motor_write",
  "params" : [
    ["speed","int","Motor speed"],
    ["time","int","Working time"]
  ]
}
Write one or more bytes
 */
void jswrap_motor_write(JsVar *parent, int speed, int time) {
	int port = jswrap_getport(parent);
	RDevMotorWrite(port, speed, time);
}

