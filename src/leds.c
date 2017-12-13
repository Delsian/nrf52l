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

static bool leds_control(ControlMessage msg)
{
	bsp_board_led_invert(1);
	switch (msg.type)
	{
	case BT_ADVERT:
		if (msg.b)
			bsp_board_led_on(ADVERTISING_LED);
		else
			bsp_board_led_off(ADVERTISING_LED);
		break;
	case BT_CONNECT:
		if (msg.b)
			bsp_board_led_on(CONNECTED_LED);
		else
			bsp_board_led_off(CONNECTED_LED);
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
