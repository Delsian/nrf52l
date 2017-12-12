/*
 * leds.c
 *
 *  Created on: Dec 11, 2017
 *      Author: Eug
 */

#include <stdint.h>
#include <stdio.h>
#include "bsp.h"
#include "control.h"
#include "FreeRTOS.h"
#include "timers.h"

#define ADVERTISING_LED 0
#define LEDBUTTON_LED 1
#define CONNECTED_LED 2

bool leds_control(ControlSignal s)
{
	switch (s)
	{
	case BT_ADV_ON:
		bsp_board_led_on(ADVERTISING_LED);
		break;
	case BT_ADV_OFF:
		bsp_board_led_off(ADVERTISING_LED);
		break;
	case BT_CONNECT:
		bsp_board_led_on(CONNECTED_LED);
		break;
	case BT_DISCONNECT:
		bsp_board_led_off(CONNECTED_LED);
		break;
	case BT_UART_RX:
		bsp_board_led_invert(LEDBUTTON_LED);
		break;
	default:
		break;
	}

	return false;
}

void leds_init()
{
    bsp_board_leds_init();
    bsp_board_leds_off();

    control_register_receiver(&leds_control);
}
