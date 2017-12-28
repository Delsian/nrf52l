// Implement flash on softdevice


#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "bluetooth.h"
#include "jsinteractive.h"
#include "jshardware.h"
#include "jstimer.h"
#include "jsutils.h"
#include "jsparse.h"
#include "jswrap_io.h"
#include "jswrap_flash.h"

bool jsfFlashContainsCode() {
	return FALSE;
}

/// Load the RAM image from flash (this is the actual interpreter state)
void jsfLoadStateFromFlash() {
  if (!jsfFlashContainsCode()) {
    jsiConsolePrintf("No code in flash!\n");
    return;
  }

}


void jsfSaveToFlash(JsvSaveFlashFlags flags, JsVar *bootCode) {
}

/** Completely clear any saved code from flash. */
void jsfRemoveCodeFromFlash() {
  jsiConsolePrint("Erasing saved code.");
}


/** Load bootup code from flash (this is textual JS code). return true if it exists and was executed.
 * isReset should be set if we're loading after a reset (eg, does the user expect this to be run or not).
 * Set isReset=false to always run the code
 */
bool jsfLoadBootCodeFromFlash(bool isReset) {
  const char *code = jsfGetBootCodeFromFlash(isReset);
  if (code)
    jsvUnLock(jspEvaluate(code, true /* We are expecting this ptr to hang around */));
  return true;
}

/** Get bootup code from flash (this is textual JS code). return a pointer to it if it exists, or 0.
 * isReset should be set if we're loading after a reset (eg, does the user expect this to be run or not).
 * Set isReset=false to always return the code  */
const char *jsfGetBootCodeFromFlash(bool isReset) {
  char *code = 0;
  if (!jsfFlashContainsCode()) return 0;

  return code;
}
