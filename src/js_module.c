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
#include "app_scheduler.h"
#include "jsparse.h"
#include "jsinteractive.h"
#include "jswrapper.h"

JsiStatus jsiStatus = 0;
JsVar *events = 0; // Array of events to execute
JsVarRef timerArray = 0; // Linked List of timers to check and run
JsSysTime jsiLastIdleTime;  ///< The last time we went around the idle loop - use this for timers
static JsSysTime jsTimeVal; // Clock in TIME_GRANULARITY*ms steps
bool interruptedDuringEvent; ///< Were we interrupted while executing an event? If so may want to clear timers

#define TIME_GRANULARITY (50)

const char script[] =
		"var m = Motor(3);\n"
		"setInterval('m.write(99,10);', 3000);";
//		"LED.off();\n"
//		"LED.set(0x55);\n"
		//"setTimeout(function () { LED.set(0x55);}, 2000);";

// Call this function every TIME_GRANULARITY ms
// do the stuff from jsinteractive.c idle loop as we have no idle here
static void JsTickHandler()
{
	jsTimeVal++;
	JsSysTime minTimeUntilNext = JSSYSTIME_MAX;
	JsSysTime timePassed = jsTimeVal - jsiLastIdleTime;
	jsiLastIdleTime = jsTimeVal;
	jsiStatus = jsiStatus & ~JSIS_TIMERS_CHANGED;
	JsVar *timerArrayPtr = jsvLock(timerArray);
	JsvObjectIterator it;
	jsvObjectIteratorNew(&it, timerArrayPtr);
	while (jsvObjectIteratorHasValue(&it) && !(jsiStatus & JSIS_TIMERS_CHANGED)) {
		bool hasDeletedTimer = false;
		JsVar *timerPtr = jsvObjectIteratorGetValue(&it);
		JsSysTime timerTime = (JsSysTime)jsvGetLongIntegerAndUnLock(jsvObjectGetChild(timerPtr, "time", 0));
		JsSysTime timeUntilNext = timerTime - timePassed;
	    if (timeUntilNext<=0) {
	        // we're now doing work
	        // jsiSetBusy(BUSY_INTERACTIVE, true); ToDo - indication
	        JsVar *timerCallback = jsvObjectGetChild(timerPtr, "callback", 0);
	        JsVar *interval = jsvObjectGetChild(timerPtr, "interval", 0);
	        JsVar *argsArray = jsvObjectGetChild(timerPtr, "args", 0);
	        bool execResult = jsiExecuteEventCallbackArgsArray(0, timerCallback, argsArray);
	        if (!execResult && interval) {
				jsErrorFlags |= JSERR_CALLBACK;
				// by setting interval to 0, we now think we've for a Timeout,
				// which will get removed.
				jsvUnLock(interval);
				interval = 0;
	        }
			if (interval) {
				timeUntilNext = timeUntilNext + jsvGetLongIntegerAndUnLock(interval);
			} else {
				// free
				// Beware... may have already been removed!
				jsvObjectIteratorRemoveAndGotoNext(&it, timerArrayPtr);
				hasDeletedTimer = true;
				timeUntilNext = -1;
			}
			jsvUnLock(timerCallback);
		}
	    // update the time until the next timer
	    if (timeUntilNext>=0 && timeUntilNext < minTimeUntilNext)
	      minTimeUntilNext = timeUntilNext;
	    // update the timer's time
	    if (!hasDeletedTimer) {
	      jsvObjectSetChildAndUnLock(timerPtr, "time", jsvNewFromLongInteger(timeUntilNext));
	      jsvObjectIteratorNext(&it);
	    }
	    jsvUnLock(timerPtr);
	}
	jsvObjectIteratorFree(&it);
	jsvUnLock(timerArrayPtr);
}

static JsVarRef _jsiInitNamedArray(const char *name) {
  JsVar *array = jsvObjectGetChild(execInfo.hiddenRoot, name, JSV_ARRAY);
  JsVarRef arrayRef = 0;
  if (array) arrayRef = jsvGetRef(jsvRef(array));
  jsvUnLock(array);
  return arrayRef;
}

void JsInit()
{
	jsvInit();
	jswInit();
	jspInit();
    jsvSoftInit();
    jspSoftInit();

    // Init clock
    APP_TIMER_DEF(tJsTimer);
    app_timer_create(&tJsTimer, APP_TIMER_MODE_REPEATED, JsTickHandler);
    app_timer_start(tJsTimer, APP_TIMER_TICKS(TIME_GRANULARITY), NULL);
    timerArray = _jsiInitNamedArray(JSI_TIMERS_NAME);
    jsiLastIdleTime = jshGetSystemTime();

	jsvUnLock(jspEvaluate(script, true));
}

JsVarInt jsiTimerAdd(JsVar *timerPtr) {
  JsVar *timerArrayPtr = jsvLock(timerArray);
  JsVarInt itemIndex = jsvArrayAddToEnd(timerArrayPtr, timerPtr, 1) - 1;
  jsvUnLock(timerArrayPtr);
  return itemIndex;
}

void jsiTimersChanged() {
  jsiStatus |= JSIS_TIMERS_CHANGED;
}
void jshSetSystemTime(JsSysTime time) { jsTimeVal = time; }
JsSysTime jshGetSystemTime() { return jsTimeVal; }
JsVarFloat jshGetMillisecondsFromTime(JsSysTime time) { return time*TIME_GRANULARITY; }
JsSysTime jshGetTimeFromMilliseconds(JsVarFloat ms) { return ms/TIME_GRANULARITY; }

NO_INLINE bool jsiExecuteEventCallbackArgsArray(JsVar *thisVar, JsVar *callbackVar, JsVar *argsArray) { // array of functions or single function
  unsigned int l = 0;
  JsVar **args = 0;
  if (argsArray) {
    assert(jsvIsArray(argsArray));
    l = (unsigned int)jsvGetArrayLength(argsArray);
    if (l) {
      args = alloca(sizeof(JsVar*) * l);
      if (!args) return false;
      jsvGetArrayItems(argsArray, l, args); // not very fast
    }
  }
  bool r = jsiExecuteEventCallback(thisVar, callbackVar, l, args);
  jsvUnLockMany(l, args);
  return r;
}

NO_INLINE bool jsiExecuteEventCallback(JsVar *thisVar, JsVar *callbackVar, unsigned int argCount, JsVar **argPtr) { // array of functions or single function
  JsVar *callbackNoNames = jsvSkipName(callbackVar);

  bool ok = true;
  if (callbackNoNames) {
    if (jsvIsArray(callbackNoNames)) {
      JsvObjectIterator it;
      jsvObjectIteratorNew(&it, callbackNoNames);
      while (ok && jsvObjectIteratorHasValue(&it)) {
        JsVar *child = jsvObjectIteratorGetValue(&it);
        ok &= jsiExecuteEventCallback(thisVar, child, argCount, argPtr);
        jsvUnLock(child);
        jsvObjectIteratorNext(&it);
      }
      jsvObjectIteratorFree(&it);
    } else if (jsvIsFunction(callbackNoNames)) {
      jsvUnLock(jspExecuteFunction(callbackNoNames, thisVar, (int)argCount, argPtr));
    } else if (jsvIsString(callbackNoNames)) {
      jsvUnLock(jspEvaluateVar(callbackNoNames, 0, 0));
    } else
      jsError("Unknown type of callback in Event Queue");
    jsvUnLock(callbackNoNames);
  }
  if (!ok || jspIsInterrupted()) {
    interruptedDuringEvent = true;
    return false;
  }
  return true;
}

/// Queue a function, string, or array (of funcs/strings) to be executed next time around the idle loop
void jsiQueueEvents(JsVar *object, JsVar *callback, JsVar **args, int argCount) { // an array of functions, a string, or a single function
  assert(argCount<10);

  JsVar *event = jsvNewObject();
  if (event) { // Could be out of memory error!
    jsvUnLock(jsvAddNamedChild(event, callback, "func"));

    if (argCount) {
      JsVar *arr = jsvNewArray(args, argCount);
      if (arr) {
        jsvUnLock2(jsvAddNamedChild(event, arr, "args"), arr);
      }
    }
    if (object) jsvUnLock(jsvAddNamedChild(event, object, "this"));

    jsvArrayPushAndUnLock(events, event);
  }
}
void jsiQueueObjectCallbacks(JsVar *object, const char *callbackName, JsVar **args, int argCount) {
	  JsVar *callback = jsvObjectGetChild(object, callbackName, 0);
	  if (!callback) return;
	  jsiQueueEvents(object, callback, args, argCount);
	  jsvUnLock(callback);
}

// === Stubs ===

NO_INLINE void jsiConsolePrintString(const char *str) {
  NRF_LOG_INFO(str);
}

void jsiConsolePrintf(const char *fmt, ...) {
  va_list argp;
  va_start(argp, fmt);
  vcbprintf((vcbprintf_callback)jsiConsolePrintString,0, fmt, argp);
  va_end(argp);
}

void jshInterruptOff() { __set_BASEPRI(4<<5); }
void jshInterruptOn() { __set_BASEPRI(0); }
bool jshIsInInterrupt() { return false; }
bool jsiFreeMoreMemory() { return false; }
void jsiConsoleRemoveInputLine() {}

unsigned int jshGetRandomNumber() {
	return 42;
}
const char *jshGetDeviceString(IOEventFlags device) { return ""; }
IOEventFlags jsiGetDeviceFromClass(JsVar *deviceClass) { return EV_NONE; }
IOEventFlags jsiGetConsoleDevice() { return 0; }
void jshGetPinString(char *result, Pin pin) {}
Pin jshGetPinFromVar(JsVar *pinv) { return -1; }
JsVar *jswrap_interface_getSerial() { return 0; }
IOEventFlags jshFromDeviceString(const char *device) { return EV_NONE; }
Pin jshGetPinFromString(const char *s) { return PIN_UNDEFINED; }


