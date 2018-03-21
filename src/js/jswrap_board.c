/*
 * !!! based on jswrap_interactive.c
 *
 * This file is part of Espruino, a JavaScript interpreter for Microcontrollers
 *
 * Copyright (C) 2013 Gordon Williams <gw@pur3.co.uk>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * ----------------------------------------------------------------------------
 * This file is designed to be parsed during the build process
 *
 * JavaScript methods and functions for the interactive shell
 * ----------------------------------------------------------------------------
 */
#include "jswrap_board.h"
#include "jsvar.h"
#include "jsflags.h"
#include "jsinteractive.h"

/*JSON{
  "type" : "function",
  "name" : "getTime",
  "generate_full" : "(JsVarFloat)jshGetSystemTime() / (JsVarFloat)jshGetTimeFromMilliseconds(1000)",
  "return" : ["float",""]
}
Return the current system time in Seconds (as a floating point number)
 */

/*JSON{
  "type" : "function",
  "name" : "setTime",
  "generate" : "jswrap_interactive_setTime",
  "params" : [
    ["time","float",""]
  ]
}
Set the current system time in seconds (to the nearest second).

This is used with `getTime`, the time reported from `setWatch`, as
well as when using `new Date()`.

To set the timezone for all new Dates, use `E.setTimeZone(hours)`.
 */
void jswrap_interactive_setTime(JsVarFloat time) {
  JsSysTime stime = jshGetTimeFromMilliseconds(time*1000);
  jshSetSystemTime(stime);
  jsiTimersChanged();
}

/*JSON{
  "type" : "function",
  "name" : "setInterval",
  "generate" : "jswrap_interface_setInterval",
  "params" : [
    ["function","JsVar","A Function or String to be executed"],
    ["timeout","float","The time between calls to the function"],
    ["args","JsVarArray","Optional arguments to pass to the function when executed"]
  ],
  "return" : ["JsVar","An ID that can be passed to clearInterval"]
}
Call the function (or evaluate the string) specified REPEATEDLY after the timeout in milliseconds.

For instance:

```
setInterval(function () {
  console.log("Hello World");
}, 1000);
// or
setInterval('console.log("Hello World");', 1000);
// both print 'Hello World' every second
```

You can also specify extra arguments that will be sent to the function when it is executed. For example:

```
setInterval(function (a,b) {
  console.log(a+" "+b);
}, 1000, "Hello", "World");
// prints 'Hello World' every second
```

If you want to stop your function from being called, pass the number that
was returned by `setInterval` into the `clearInterval` function.

 **Note:** If `setDeepSleep(true)` has been called and the interval is greater than 5 seconds, Espruino may execute the interval up to 1 second late. This is because Espruino can only wake from deep sleep every second - and waking early would cause Espruino to waste power while it waited for the correct time.
 */
/*JSON{
  "type" : "function",
  "name" : "setTimeout",
  "generate" : "jswrap_interface_setTimeout",
  "params" : [
    ["function","JsVar","A Function or String to be executed"],
    ["timeout","float","The time until the function will be executed"],
    ["args","JsVarArray","Optional arguments to pass to the function when executed"]
  ],
  "return" : ["JsVar","An ID that can be passed to clearTimeout"]
}
Call the function (or evaluate the string) specified ONCE after the timeout in milliseconds.

For instance:

```
setTimeout(function () {
  console.log("Hello World");
}, 1000);
// or
setTimeout('console.log("Hello World");', 1000);
// both print 'Hello World' after a second
```

You can also specify extra arguments that will be sent to the function when it is executed. For example:

```
setTimeout(function (a,b) {
  console.log(a+" "+b);
}, 1000, "Hello", "World");
// prints 'Hello World' after 1 second
```

If you want to stop the function from being called, pass the number that
was returned by `setTimeout` into the `clearInterval` function.

 **Note:** If `setDeepSleep(true)` has been called and the interval is greater than 5 seconds, Espruino may execute the interval up to 1 second late. This is because Espruino can only wake from deep sleep every second - and waking early would cause Espruino to waste power while it waited for the correct time.
 */
JsVar *_jswrap_interface_setTimeoutOrInterval(JsVar *func, JsVarFloat interval, JsVar *args, bool isTimeout) {
  // NOTE: The 5 sec delay mentioned in the description is handled by jshSleep
  JsVar *itemIndex = 0;
  if (!jsvIsFunction(func) && !jsvIsString(func)) {
    jsExceptionHere(JSET_ERROR, "Function or String not supplied!");
  } else {
    // Create a new timer
    JsVar *timerPtr = jsvNewObject();
    if (isnan(interval) || interval<TIMER_MIN_INTERVAL) interval=TIMER_MIN_INTERVAL;
    JsSysTime intervalInt = jshGetTimeFromMilliseconds(interval);
    jsvObjectSetChildAndUnLock(timerPtr, "time", jsvNewFromLongInteger(jshGetSystemTime() + intervalInt));
    if (!isTimeout) {
      jsvObjectSetChildAndUnLock(timerPtr, "interval", jsvNewFromLongInteger(intervalInt));
    }
    jsvObjectSetChild(timerPtr, "callback", func); // intentionally no unlock
    if (jsvGetArrayLength(args))
      jsvObjectSetChild(timerPtr, "args", args); // intentionally no unlock

    // Add to array
    itemIndex = jsvNewFromInteger(jsiTimerAdd(timerPtr));
    jsvUnLock(timerPtr);
    jsiTimersChanged(); // mark timers as changed
  }
  return itemIndex;
}
JsVar *jswrap_interface_setInterval(JsVar *func, JsVarFloat timeout, JsVar *args) {
  return _jswrap_interface_setTimeoutOrInterval(func, timeout, args, false);
}
JsVar *jswrap_interface_setTimeout(JsVar *func, JsVarFloat timeout, JsVar *args) {
  return _jswrap_interface_setTimeoutOrInterval(func, timeout, args, true);
}

/*JSON{
  "type" : "function",
  "name" : "clearInterval",
  "generate" : "jswrap_interface_clearInterval",
  "params" : [
    ["id","JsVar","The id returned by a previous call to setInterval"]
  ]
}
Clear the Interval that was created with setInterval, for example:

```var id = setInterval(function () { print('foo'); }, 1000);```

```clearInterval(id);```

If no argument is supplied, all timers and intervals are stopped
 */
/*JSON{
  "type" : "function",
  "name" : "clearTimeout",
  "generate" : "jswrap_interface_clearTimeout",
  "params" : [
    ["id","JsVar","The id returned by a previous call to setTimeout"]
  ]
}
Clear the Timeout that was created with setTimeout, for example:

```var id = setTimeout(function () { print('foo'); }, 1000);```

```clearTimeout(id);```

If no argument is supplied, all timers and intervals are stopped
 */
void _jswrap_interface_clearTimeoutOrInterval(JsVar *idVar, bool isTimeout) {
  JsVar *timerArrayPtr = jsvLock(timerArray);
  if (jsvIsUndefined(idVar)) {
    /* Delete all timers EXCEPT those with a 'watch' field,
     as those were generated by jsinteractive.c for debouncing watches */
    JsvObjectIterator it;
    jsvObjectIteratorNew(&it, timerArrayPtr);
    while (jsvObjectIteratorHasValue(&it)) {
      JsVar *timerPtr = jsvObjectIteratorGetValue(&it);
      JsVar *watchPtr = jsvObjectGetChild(timerPtr, "watch", 0);
      if (!watchPtr)
        jsvObjectIteratorRemoveAndGotoNext(&it, timerArrayPtr);
      else
        jsvObjectIteratorNext(&it); 
      jsvUnLock2(watchPtr, timerPtr);
    }
    jsvObjectIteratorFree(&it);
  } else {
    JsVar *child = jsvIsBasic(idVar) ? jsvFindChildFromVar(timerArrayPtr, idVar, false) : 0;
    if (child) {
      JsVar *timerArrayPtr = jsvLock(timerArray);
      jsvRemoveChild(timerArrayPtr, child);
      jsvUnLock2(child, timerArrayPtr);
    } else {
      if (isTimeout)
        jsExceptionHere(JSET_ERROR, "Unknown Timeout");
      else
        jsExceptionHere(JSET_ERROR, "Unknown Interval");
    }
  }
  jsvUnLock(timerArrayPtr);
  jsiTimersChanged(); // mark timers as changed
}
void jswrap_interface_clearInterval(JsVar *idVar) {
  _jswrap_interface_clearTimeoutOrInterval(idVar, false);
}
void jswrap_interface_clearTimeout(JsVar *idVar) {
  _jswrap_interface_clearTimeoutOrInterval(idVar, true);
}

/*JSON{
  "type" : "function",
  "name" : "changeInterval",
  "generate" : "jswrap_interface_changeInterval",
  "params" : [
    ["id","JsVar","The id returned by a previous call to setInterval"],
    ["time","float","The new time period in ms"]
  ]
}
Change the Interval on a callback created with setInterval, for example:

```var id = setInterval(function () { print('foo'); }, 1000); // every second```

```changeInterval(id, 1500); // now runs every 1.5 seconds```

This takes effect immediately and resets the timeout, so in the example above,
regardless of when you call `changeInterval`, the next interval will occur 1500ms
after it.
 */
void jswrap_interface_changeInterval(JsVar *idVar, JsVarFloat interval) {
  JsVar *timerArrayPtr = jsvLock(timerArray);
  if (interval<TIMER_MIN_INTERVAL) interval=TIMER_MIN_INTERVAL;
  JsVar *timerName = jsvIsBasic(idVar) ? jsvFindChildFromVar(timerArrayPtr, idVar, false) : 0;
  if (timerName) {
    JsVar *timer = jsvSkipNameAndUnLock(timerName);
    JsVar *v;
    JsVarInt intervalInt = (JsVarInt)jshGetTimeFromMilliseconds(interval);
    v = jsvNewFromInteger(intervalInt);
    jsvUnLock2(jsvSetNamedChild(timer, v, "interval"), v);
    v = jsvNewFromInteger(jshGetSystemTime() + intervalInt);
    jsvUnLock3(jsvSetNamedChild(timer, v, "time"), v, timer);
    // timerName already unlocked
    jsiTimersChanged(); // mark timers as changed
  } else {
    jsExceptionHere(JSET_ERROR, "Unknown Interval");
  }
  jsvUnLock(timerArrayPtr);
}
