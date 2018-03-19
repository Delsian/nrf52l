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
#include "jswrap_led.h"
#include "jsdevices.h"
#include "r0b1c_device.h"
#include "rdev_proto.h"
#include "nrf_log.h"

/*JSON{
  "type" : "class",
  "class" : "Motor"
}
This class provides Motor control
 */

static int motor_getport(JsVar *parent) {
  return jsvGetIntegerAndUnLock(jsvObjectGetChild(parent, "port", 0));
}

/*JSON{
  "type" : "constructor",
  "class" : "Motor",
  "name" : "Motor",
  "generate" : "jswrap_motor_constructor",
  "params" : [
    ["port","int","The port button connected to"]
  ],
  "return" : ["JsVar","A Motor object"]
}
Create a software OneWire implementation on the given pin
 */
JsVar *jswrap_motor_constructor(int port) {
  JsVar *mo = jspNewObject(0, "Motor");
  if (mo && RDeviceChange(port, RDEV_MOTOR_M) == RDERR_OK && RDevMotorMInit(port) == RDERR_OK) { // ToDo motor size
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
	int port = motor_getport(parent);
	RDevMotorWrite(port, speed, time);
}

