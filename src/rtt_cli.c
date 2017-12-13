/*
 * rtt_cli.c
 *
 *  Created on: 13 груд. 2017 р.
 *      Author: ekrashtan
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "control.h"
#include "bsp.h"
#include "bluetooth.h"

#include "SEGGER_RTT.h"
#define RX_BUF_LEN 32

#ifdef CLIRTT

static bool rtt_cli_control(ControlMessage msg)
{
	if (msg.type == BT_UART_RX)
	{
		uint8_t* buf = msg.ptr;
		SEGGER_RTT_Write(0, buf+2, *((uint16_t*)buf));
		SEGGER_RTT_Write(0, "\r\n", 3);
		return true;
	}
	return false;
}

void rtt_cli_init()
{
	SEGGER_RTT_Init();
	SEGGER_RTT_WriteString(0,"CLI>\r\n");
	control_register_receiver(&rtt_cli_control);
}

void rtt_cli_thread(void * arg)
{
	//uint8_t buffer[RX_BUF_LEN];
	//uint16_t rd_len;

	while(1)
	{
//		if(SEGGER_RTT_HasData(0))
//		{
//			rd_len = SEGGER_RTT_Read(0, buffer, RX_BUF_LEN);
//			bluetooth_send(buffer, rd_len);
//			SEGGER_RTT_WriteString(0,"Ok>\r\n");
//		}
		vTaskDelay(100);
		bsp_board_led_invert(1);
	}
}


int _write_r(struct _reent *r, int file, char *ptr, int len) {
	SEGGER_RTT_Write(0, ptr, len);
	SEGGER_RTT_Write(0, "\r\n", 3);
    return len;
}


#endif // CLIRTT
