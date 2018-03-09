/*
 * battery.c
 *
 *  Created on: Jan 20, 2018
 *      Author: Eug
 */

#include <stdbool.h>
#include <stdint.h>
#include "nrf_log.h"
#include "boards.h"
#include "nrf_drv_saadc.h"
#include "nrf_error.h"
#include "control.h"
#include "r0b1c_device.h"
#include "r0b1c_service.h"
#include "r0b1c_cmd.h"
#include "rdev_led.h"

#define BATTERY_MEASURE_TICKS 5000
#define BATTERY_GAIN NRF_SAADC_GAIN1_3

typedef enum { BSNONE, BSCHRG, BSSTDBY, BSEMPTY } BatStates;

static uint16_t guBattValue;
static nrf_saadc_value_t BattBuffer;
static BatStates tBstate;

const ControlEvent BattEvt = {
		.type = CE_BATT_IN,
		.ptr16 = &guBattValue
};

static void BatteryStateChange (BatStates b) {

	if (tBstate != b) {
		NRF_LOG_DEBUG("Batt new state %d", b);

		// Clean old indication
		switch(tBstate) {
		case BSCHRG:
			RDevLedClearIndication(LED_IND_CHARGING);
			break;
		case BSSTDBY:
			RDevLedClearIndication(LED_IND_CHARGED);
			break;
		case BSEMPTY:
			RDevLedClearIndication(LED_IND_LOWBATT);
			break;
		default:
			break;
		}

		// Indicate new state
		switch(b) {
		case BSCHRG:
			RDevLedSetIndication(LED_IND_CHARGING);
			SendBatteryNotification(4);
			break;
		case BSSTDBY:
			RDevLedSetIndication(LED_IND_CHARGED);
			SendBatteryNotification(2);
			break;
		case BSEMPTY:
			RDevLedSetIndication(LED_IND_LOWBATT);
			SendBatteryNotification(1);
			break;
		default:
			break;
		}
	}
}

void BatteryMeasureCb(nrf_drv_saadc_evt_t const * p_event)
{
	if (p_event->type == NRF_DRV_SAADC_EVT_DONE)
	{
		guBattValue = BattBuffer;
		ret_code_t err_code = nrf_drv_saadc_buffer_convert(&BattBuffer, 1);
		//if(err_code) printf("%d Err %x\n", __LINE__, err_code);
		ControlPost(&BattEvt);
	}
}

RDevErrCode BatteryTick(uint8_t port, uint32_t time)
{
	static uint16_t ticks;


	if (nrf_gpio_pin_read(PIN_CHRG) == 0) {
		BatteryStateChange(BSCHRG);
	} else if (nrf_gpio_pin_read(PIN_STDBY) == 0) {
		BatteryStateChange(BSSTDBY);
	} else {
		// no signals from charger
		if (tBstate == BSCHRG || tBstate == BSSTDBY ) {
			BatteryStateChange(BSNONE);
		}
	}
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

    // Set pins to read charger state
    nrf_gpio_cfg_input(PIN_CHRG, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(PIN_STDBY, NRF_GPIO_PIN_PULLUP);

	err_code = nrf_drv_saadc_init(NULL, BatteryMeasureCb);
	APP_ERROR_CHECK(err_code);

	err_code = nrf_drv_saadc_channel_init(0, &channel_config);
	APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_saadc_buffer_convert(&BattBuffer, 1);
    APP_ERROR_CHECK(err_code);

    return RDERR_OK;
}

RDevErrCode RDevBattCmd(const uint8_t* pData, uint8_t len)
{
	RDevErrCode tErr = RDERR_NOT_SUPPORTED;
	RDevCmdCode ubCommand = (RDevCmdCode)pData[1];
	switch (ubCommand) {
	case RDCMD_SET:
		// Write new calibration value
		tErr = RDERR_DONE;
		break;
	case RDCMD_GET:
		{
			uint8_t pubResp[4] = {RDEV_BATTERY, RDCMD_GET, guBattValue&0xFF, guBattValue>>8 };
			SendCmdResp(pubResp, 4);
			tErr = RDERR_OK;
		}
		break;
	default:
		break;
	}

	return tErr;
}
