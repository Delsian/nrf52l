/*
 * battery.c
 *
 *  Created on: Jan 20, 2018
 *      Author: Eug
 */

#include <stdbool.h>
#include <stdint.h>
#include "nrf_gpio.h"
#include "boards.h"
#include "app_timer.h"
#include "nrf_drv_saadc.h"
#include "nrf_error.h"
#include "control.h"

#define BATTERY_MEASURE_TIMEOUT 250000
#define BATTERY_GAIN NRF_SAADC_GAIN1_3

static uint8_t guBattValue;
static nrf_saadc_value_t BattBuffer;

const ControlEvent BattEvt = {
		.type = CE_BATT_IN,
		.ptr8 = &guBattValue
};
/**
 * Keep powered after pressing button 'On'
 */
static void BatteryOn()
{
	nrf_gpio_cfg_output(PWR_ON);
	nrf_gpio_pin_set(PWR_ON);
}

void BatteryOff()
{
	nrf_gpio_pin_clear(PWR_ON);
}

void BatteryMeasureCb(nrf_drv_saadc_evt_t const * p_event)
{
	if (p_event->type == NRF_DRV_SAADC_EVT_DONE)
	{
		guBattValue = (BattBuffer>>4)&0xFF;
		ret_code_t err_code = nrf_drv_saadc_buffer_convert(&BattBuffer, 1);
		//if(err_code) printf("%d Err %x\n", __LINE__, err_code);
		ControlPost(&BattEvt);
	}
}

static void BatteryTmrHandler()
{
	ret_code_t err_code = nrf_drv_saadc_sample();
	//if(err_code) printf("%d Err %x\n", __LINE__, err_code);
}

void BatteryInit()
{
    ret_code_t err_code;
    nrf_saadc_channel_config_t channel_config =
        NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(VBAT_AIN);
    channel_config.gain = BATTERY_GAIN;
    channel_config.acq_time = NRF_SAADC_ACQTIME_20US;

	BatteryOn();

	err_code = nrf_drv_saadc_init(NULL, BatteryMeasureCb);
	APP_ERROR_CHECK(err_code);

	err_code = nrf_drv_saadc_channel_init(0, &channel_config);
	APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_saadc_buffer_convert(&BattBuffer, 1);
    APP_ERROR_CHECK(err_code);

	APP_TIMER_DEF(bat_timer);
	err_code = app_timer_create(&bat_timer, APP_TIMER_MODE_REPEATED, BatteryTmrHandler);
	err_code = app_timer_start(bat_timer, BATTERY_MEASURE_TIMEOUT, NULL);
}
