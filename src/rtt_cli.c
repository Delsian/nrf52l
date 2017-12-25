/*
 * rtt_cli.c
 *
 *  Created on: 13 груд. 2017 р.
 *      Author: ekrashtan
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "bsp.h"
#include "bluetooth.h"

#include "SEGGER_RTT.h"
#define RX_BUF_LEN 8

#ifdef CLIRTT

void rtt_cli_init()
{
	SEGGER_RTT_Init();
	SEGGER_RTT_WriteString(0,"CLI>\n");
}

void rtt_cli_thread(void * arg)
{
	uint8_t buffer[RX_BUF_LEN];
	int key;
	uint8_t index = 0;

	while(1)
	{
		if((key = SEGGER_RTT_GetKey()) > 0)
		{
			buffer[index++] = (uint8_t)key;
			if (key == '\r' || key == '\n' || index == RX_BUF_LEN)
			{
				//bluetooth_send(buffer, index);
				index = 0;
			}
		}
		else
		{
			//vTaskDelay(100);
		}
		//bsp_board_led_invert(1);
	}
}


int _write_r(struct _reent *r, int file, char *ptr, int len) {
	SEGGER_RTT_Write(0, ptr, (ptr[len-1] == '\n')?len-1:len);
	SEGGER_RTT_Write(0, "\n", 2);
    return len;
}


#endif // CLIRTT
