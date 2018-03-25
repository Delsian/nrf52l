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
#include "ble_types.h"
#include "app_timer.h"
#include "control.h"
#include "r0b1c_device.h"
#include "r0b1c_service.h"
#include "r0b1c_cmd.h"
#include "rdev_led.h"

#define BATTERY_MEASURE_TICKS 5000
#define BATTERY_GAIN NRF_SAADC_GAIN1_3

#define BATTERY_IDLE_PWROFF_TIMEOUT 5000000
#define BATTERY_EMPTY_PWROFF_TIMEOUT 3500000

typedef enum { BSNONE, BSCHRG, BSSTDBY, BSBATT, BSEMPTY } BatStates;

static uint16_t guBattValue;
static nrf_saadc_value_t BattBuffer;
static BatStates tBstate;
static bool eDisablePwrOff;
APP_TIMER_DEF(tPwrOffTmr);

const ControlEvent BattEvt = {
		.type = CE_BATT_IN,
		.ptr16 = &guBattValue
};

static void BatteryStateChange (BatStates b) {

	if (tBstate != b) {
		NRF_LOG_DEBUG("Batt new state %d, old %d", b, tBstate);

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
			// disable pwr-off timer
			app_timer_stop(tPwrOffTmr);
			break;
		case BSBATT:
			// disable pwr-off timer
			app_timer_stop(tPwrOffTmr);
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
			// ToDo calibrate
			break;
		case BSEMPTY:
			RDevLedSetIndication(LED_IND_LOWBATT);
			SendBatteryNotification(1);
			break;
		case BSBATT:
			SendBatteryNotification(0);
			if(!eDisablePwrOff) {
				// Charger disconnected, set timer to power off
				NRF_LOG_DEBUG("Power off in %ld ticks", BATTERY_IDLE_PWROFF_TIMEOUT);
				app_timer_start(tPwrOffTmr, BATTERY_IDLE_PWROFF_TIMEOUT, NULL);
			}
			break;
		default:
			break;
		}
		tBstate = b;
	}
}

void BatteryMeasureCb(nrf_drv_saadc_evt_t const * p_event)
{
	if (p_event->type == NRF_DRV_SAADC_EVT_DONE)
	{
		guBattValue = BattBuffer;
		nrf_drv_saadc_buffer_convert(&BattBuffer, 1);
		// Calculate current level and set BSEMPTY if necessary

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
			if (tBstate != BSEMPTY)
				BatteryStateChange(BSBATT);
		}
	}
	if (BATTERY_MEASURE_TICKS < ++ticks) {
		nrf_drv_saadc_sample();
		ticks = 0;
	}

	return RDERR_OK;
}

static void BatteryPwrOffCb()
{
	const ControlEvent tPwr = {
			.type = CE_PWR_OFF_REQ
	};
	if(!eDisablePwrOff) {
		ControlPost(&tPwr);
	}
}
static void BatteryReqCb(const ControlEvent* pEvt)
{
	const ControlEvent tPwr = {
			.type = CE_PWR_OFF
	};

	if (pEvt->type == CE_BT_CONN) {
		eDisablePwrOff = (*(pEvt->ptr16) != BLE_CONN_HANDLE_INVALID);
		if (!eDisablePwrOff && (tBstate==BSBATT || tBstate==BSEMPTY)) {
			uint32_t timeout = (tBstate==BSBATT)?BATTERY_IDLE_PWROFF_TIMEOUT:BATTERY_EMPTY_PWROFF_TIMEOUT;
			NRF_LOG_DEBUG("Power off in %ld ticks", timeout);
			app_timer_start(tPwrOffTmr, timeout, NULL);
		}
		if(eDisablePwrOff) {
			app_timer_stop(tPwrOffTmr);
		}
	} else if (pEvt->type == CE_PWR_OFF_REQ) { // If not in charging mode
		if (tBstate != BSCHRG && tBstate != BSSTDBY ) {
			ControlPost(&tPwr);
		} else {
			// ToDo: negative buzz
		}
	}
}

bool BatteryIfPwrOffEnabled() {
	return (tBstate != BSCHRG && tBstate != BSSTDBY );
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

    // Subscribe to connect event for inactivity timer enable/disable
    ControlRegisterCb(CE_BT_CONN|CE_PWR_OFF_REQ, &BatteryReqCb);
    app_timer_create(&tPwrOffTmr, APP_TIMER_MODE_SINGLE_SHOT, BatteryPwrOffCb);

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
