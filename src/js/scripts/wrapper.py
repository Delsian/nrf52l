#!/bin/false

SUBMODULE = "../../Espruino/src"

wrapmodules = "python build_jswrapper.py "+\
    SUBMODULE+"/jswrap_array.c "+\
    SUBMODULE+"/jswrap_arraybuffer.c "+\
    SUBMODULE+"/jswrap_dataview.c "+\
    SUBMODULE+"/jswrap_date.c "+\
    SUBMODULE+"/jswrap_error.c "+\
    SUBMODULE+"/jswrap_functions.c "+\
    SUBMODULE+"/jswrap_json.c "+\
    "jswrap_math.c "+\
    SUBMODULE+"/jswrap_modules.c "+\
    SUBMODULE+"/jswrap_number.c "+\
    SUBMODULE+"/jswrap_object.c "+\
    SUBMODULE+"/jswrap_process.c "+\
    SUBMODULE+"/jswrap_promise.c "+\
    SUBMODULE+"/jswrap_regexp.c "+\
    SUBMODULE+"/jswrap_string.c "+\
    "jswrap_board.c "+\
    "../devices/jswrap_motor.c "+\
    "../devices/jswrap_led.c "+\
    "../devices/jswrap_range.c "+\
    "-BESP32 -Fjswrapper.c"

if __name__ == '__main__':
    import os
    os.system(wrapmodules)    
