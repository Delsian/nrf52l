/*
 * leds.c
 *
 *  Created on: Dec 11, 2017
 *      Author: Eug
 */

#include <stdint.h>
#include <stdio.h>
#include "bsp.h"
#include "leds.h"

#define ADVERTISING_LED 0
#define LEDBUTTON_LED 1
#define CONNECTED_LED 2

void leds_scheduler(void * p_event_data, uint16_t event_size)
{
    UNUSED_PARAMETER(event_size);
	LedsControlSignal signal = *((LedsControlSignal*)p_event_data);
	switch (signal)
	{
	case LED1_ON:
		bsp_board_led_on(ADVERTISING_LED);
		break;
	case LED1_OFF:
		bsp_board_led_off(ADVERTISING_LED);
		break;
	case LED2_ON:
		bsp_board_led_on(CONNECTED_LED);
		break;
	case LED2_OFF:
		bsp_board_led_off(CONNECTED_LED);
		break;
	case LED3_ON:
		bsp_board_led_on(LEDBUTTON_LED);
		break;
	case LED3_OFF:
		bsp_board_led_off(LEDBUTTON_LED);
		break;
	default:
		break;
	}
}

void leds_init()
{
    bsp_board_leds_init();
    bsp_board_leds_off();
}
