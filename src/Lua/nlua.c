/*
 * lua.c
 *
 *  Created on: 14 груд. 2017 р.
 *      Author: ekrashtan
 */

#include <stdint.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "control.h"

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

static void lua_thread(void * arg)
{

	lua_State *L = lua_open();
	if (L == NULL) {
		APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
	}

	while(1)
	{

	}
}

void nlua_init()
{
    TaskHandle_t  elua_th;


    if(pdPASS != xTaskCreate(lua_thread, "LUA", 256, NULL, 1, &elua_th))
    {
        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
    }
}

int fileno(void *stream)
{
	return 0;
}
