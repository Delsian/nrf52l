/*
 * battery.c
 *
 *  Created on: Jan 20, 2018
 *      Author: Eug
 */

#include <stdbool.h>
#include <stdint.h>
#include "boards.h"
#include "nrf_drv_saadc.h"
#include "nrf_error.h"
#include "control.h"
#include "r0b1c_device.h"

#define BATTERY_MEASURE_TICKS 5000
#define BATTERY_GAIN NRF_SAADC_GAIN1_3

static uint8_t guBattValue;
static nrf_saadc_value_t BattBuffer;

const ControlEvent BattEvt = {
		.type = CE_BATT_IN,
		.ptr8 = &guBattValue
};


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

RDevErrCode BatteryTick(uint8_t port, uint32_t time)
{
	static uint16_t ticks;

	if (BATTERY_MEASURE_TICKS < ++ticks) {
		nrf_drv_saadc_sample();
		ticks = 0;
	}

	return RDERR_OK;
}

RDevErrCode BatteryInit(uint8_t port)
{
    ret_code_t err_code;
    nrf_saadc_channel_config_t channel_config =
        NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(VBAT_AIN);
    channel_config.gain = BATTERY_GAIN;
    channel_config.acq_time = NRF_SAADC_ACQTIME_20US;

	err_code = nrf_drv_saadc_init(NULL, BatteryMeasureCb);
	APP_ERROR_CHECK(err_code);

	err_code = nrf_drv_saadc_channel_init(0, &channel_config);
	APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_saadc_buffer_convert(&BattBuffer, 1);
    APP_ERROR_CHECK(err_code);

    return RDERR_OK;
}
