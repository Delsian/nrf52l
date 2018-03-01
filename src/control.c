/*
 * control.c
 *
 *  Created on: Dec 11, 2017
 *      Author: Eug
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "control.h"
#include "nrf_queue.h"
#include "nrf_gpio.h"
#include "app_scheduler.h"
#include "nrf_log.h"

#include "boards.h"
#include "pca9685.h"
#include "custom_service.h"
#include "rdev_led.c"

extern void battery_level_update(uint8_t battery_level);
static LedPatternSeq* ptKeepPatt;

static void ControlRestoreLedPattern()
{
	if (ptKeepPatt) {
		RDevLedSetPattern(ptKeepPatt);
		ptKeepPatt = NULL;
	}
}

const LedPatternSeq BtConnLed = {
		.repeats = 6,
		.length = 2,
		.patEnd = &ControlRestoreLedPattern,
		.pt = { {0x10, COLOR_BLUE },
				{0x20, COLOR_BLACK }}
};

static void ControlEvtH(void * p_evt, uint16_t size)
{
	ControlEvent* iEvt = (ControlEvent*)p_evt;
	NRF_LOG_DEBUG("Evt type %d", iEvt->type);
	switch (iEvt->type)
	{
	case CE_BATT_IN:
	    battery_level_update(*(iEvt->ptr8));
	    // Check if battery low
		//WedoBattery(iEvt->ptr8);
		break;
	case CE_LED_CHG:
		PcaLedColor(*(iEvt->ptr8));
		break;
	case CE_PWR_OFF:
		nrf_gpio_pin_clear(PWR_ON);
		break;
	case CE_DEVNAME_CHG:
		CustSetDeviceName(iEvt->ptr8);
		break;
	case CE_BT_CONN:
		ptKeepPatt = RDevLedSetPattern(&BtConnLed);
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
