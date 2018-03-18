/*
 * js_module.c
 *
 *  Created on: 16.03.2018
 *      Author: Eug
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "nrf_log.h"
#include "app_timer.h"
#include "jsparse.h"
#include "jsinteractive.h"
#include "jswrapper.h"

JsiStatus jsiStatus = 0;

const char script[] =
		"function test() {\n"
		"var i= new LED();\n"
		"i.set(100);}\n"
		"test();";

void js_run()
{
	jsvInit();
	jswInit();
	jspInit();
    jsvSoftInit();
    jspSoftInit();
	jsvUnLock(jspEvaluate(script, true));
}

NO_INLINE void jsiConsolePrintString(const char *str) {
  NRF_LOG_INFO(str);
}

void jsiConsolePrintf(const char *fmt, ...) {
  va_list argp;
  va_start(argp, fmt);
  vcbprintf((vcbprintf_callback)jsiConsolePrintString,0, fmt, argp);
  va_end(argp);
}

void jshInterruptOff() {
  __set_BASEPRI(4<<5);
}
void jshInterruptOn() { __set_BASEPRI(0); }
bool jshIsInInterrupt() { return false; }
bool jsiFreeMoreMemory() { return false; }
void jsiConsoleRemoveInputLine() {}

unsigned int jshGetRandomNumber() {
	return 42;
}

/// Queue a function, string, or array (of funcs/strings) to be executed next time around the idle loop
void jsiQueueEvents(JsVar *object, JsVar *callback, JsVar **args, int argCount) { // an array of functions, a string, or a single function

}
void jsiQueueObjectCallbacks(JsVar *object, const char *callbackName, JsVar **args, int argCount) {
}

JsSysTime jshGetSystemTime() {
	return app_timer_cnt_get();
}

const char *jshGetDeviceString(IOEventFlags device) { return ""; }
IOEventFlags jsiGetDeviceFromClass(JsVar *deviceClass) { return EV_NONE; }
IOEventFlags jsiGetConsoleDevice() { return 0; }
void jshGetPinString(char *result, Pin pin) {}
Pin jshGetPinFromVar(JsVar *pinv) { return -1; }
JsSysTime jshGetTimeFromMilliseconds(JsVarFloat ms) { return ms*32; }
JsVar *jswrap_interface_getSerial() { return 0; }
IOEventFlags jshFromDeviceString(const char *device) { return EV_NONE; }
JsVarFloat jshGetMillisecondsFromTime(JsSysTime time) { return time/32; }
Pin jshGetPinFromString(const char *s) { return 0xFF; }


