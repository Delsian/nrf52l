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
#include "jsparse.h"
#include "jsinteractive.h"

JsiStatus jsiStatus = 0;

const char script[] =
		"var i=12;\n"
		"Port.set(i);\n";

void js_run()
{
	jsvInit();
	jspInit();
	JsVar *evCode = jsvNewFromString(script);
	JsVar *moduleExport = jspEvaluateModule(evCode);
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

void jshInterruptOn() {
  __set_BASEPRI(0);
}

bool jsiFreeMoreMemory() {
	return false;
}

void jsiConsoleRemoveInputLine() {}

unsigned int jshGetRandomNumber() {
	return 42;
}

/// Queue a function, string, or array (of funcs/strings) to be executed next time around the idle loop
void jsiQueueEvents(JsVar *object, JsVar *callback, JsVar **args, int argCount) { // an array of functions, a string, or a single function

}
void jsiQueueObjectCallbacks(JsVar *object, const char *callbackName, JsVar **args, int argCount) {
}
