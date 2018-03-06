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
#include "nrf_delay.h"

#include "boards.h"
#include "pca9685.h"
#include "custom_service.h"
#include "rdev_led.h"
#include "buzzer.h"

extern void battery_level_update(uint8_t battery_level);

static void CntPwrOff()
{
    BuzzerPlayTone(500);
    nrf_delay_ms(50);
    BuzzerPlayTone(700);
    nrf_delay_ms(70);
    BuzzerPlayTone(0);
	// ToDo: store settings to flash
	nrf_gpio_pin_clear(PWR_ON);
}

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
	case CE_LED_SET:
		RDevLedSetPattern((LedPatternSeq*)(iEvt->ptr));
		break;
	case CE_PWR_OFF:
		CntPwrOff();
		break;
	case CE_DEVNAME_CHG:
		CustSetDeviceName(iEvt->ptr8);
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
