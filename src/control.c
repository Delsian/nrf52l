/*
 * control.c
 *
 *  Created on: Dec 11, 2017
 *      Author: Eug
 */

#include <stdint.h>
#include <stdio.h>
#include "control.h"
#include "nrf_queue.h"
#include "app_scheduler.h"

static void ControlEvtH(void * p_evt, uint16_t size)
{
	ControlEvent* iEvt = (ControlEvent*)p_evt;
	switch (iEvt->type)
	{
	case CE_BATT_IN:
		printf("Batt %d\n", *(iEvt->ptr8));
		break;
	case CE_LED_CHG:
		break;
	default:
		break;
	}
}

void ControlPost(const ControlEvent* evt)
{
	app_sched_event_put(evt, sizeof(ControlEvent), ControlEvtH);
}

void ControlInit()
{
    APP_SCHED_INIT(sizeof(ControlEvent), 8);

}
