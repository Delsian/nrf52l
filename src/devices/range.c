/*
 * range.c
 *
 *
 *  Created on: 12-03-18
 *      Author: ekrashtan
 */

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "rj_port.h"
#include "nrf_delay.h"
#include "nrf_log.h"
#include "app_timer.h"
#include "app_scheduler.h"
#include "nrf_drv_gpiote.h"
#include "r0b1c_device.h"
#include "r0b1c_cmd.h"

// These values measured individually. ToDo: calibration procedure
#define DIFF_MIN_VALUE 12
#define DIFF_MAX_VALUE 120

static uint8_t ubPortPlus1; // 0 (default value after startup) means "module not initialized"
static uint8_t ubRange; // keeping range value
volatile static bool isWaiting; // Indicates active cycle
volatile static uint32_t ulRtcCnt;

static void RDevRengeNotify(void* ipData, uint16_t size)
{
	uint8_t notif[3] = {ubPortPlus1-1, RDCMD_GET, ubRange};
	SendCmdNotif(notif, 3);
}

static void RDevRangeToggle(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	if(action==NRF_GPIOTE_POLARITY_TOGGLE && ubPortPlus1 && isWaiting) {
		if (nrf_gpio_pin_read(pin) == 0) {
			uint32_t diff = app_timer_cnt_diff_compute(app_timer_cnt_get(), ulRtcCnt);
			isWaiting = false;
			NRF_LOG_DEBUG("L=%d", diff);
			if (diff > DIFF_MIN_VALUE && diff < DIFF_MAX_VALUE) {
				ubRange =  (uint8_t)(diff - DIFF_MIN_VALUE);
				app_sched_event_put(&ubRange, sizeof(uint8_t*), RDevRengeNotify);
			}
		}
	}
}

// Yellow - trigger
// Blue - Echo
RDevErrCode RDevRangeInit(uint8_t port)
{
	if (ubPortPlus1) return RDERR_BUSY; // Current version supports only one range sensor per module

	ubPortPlus1 = port+1;
	RjPortSetOutput(port, PinYellow);
	RjPortPinSet(port, PinYellow, 0);

	nrf_drv_gpiote_in_config_t conf = {
			.is_watcher = false,                     \
			.hi_accuracy = true,                  \
			.pull = NRF_GPIO_PIN_PULLUP,             \
			.sense = NRF_GPIOTE_POLARITY_TOGGLE
	};
	uint32_t pin = RjPortGetPinNum(port, PinBlue);
	nrf_drv_gpiote_in_init( pin, &conf, &RDevRangeToggle);
	isWaiting = false;
	return RDERR_OK;
}

RDevErrCode RDevRangeUnInit(uint8_t port)
{
	nrf_drv_gpiote_in_event_disable(RjPortGetPinNum(port, PinBlue));
	nrf_drv_gpiote_in_uninit(RjPortGetPinNum(port, PinBlue));
	ubPortPlus1 = 0; // Set to 0 as marker for empty module
	return RDERR_OK;
}

RDevErrCode RDevRangeTick(uint8_t port, uint32_t time)
{
	if ((time & 0x7F) == 25) { // checking active port every 128th tick, use random shift from timer start
		if (isWaiting) {
			// no response from previous ping
			ubRange = 0xFF;
		}

		RjPortPinSet(port, PinYellow, 1);
		nrf_delay_us(10);
		RjPortPinSet(port, PinYellow, 0);
		ulRtcCnt = app_timer_cnt_get();
		isWaiting = true;
	}
	return RDERR_DONE;
}

RDevErrCode RDevRangeCmd(const uint8_t* pData, uint8_t len)
{
	uint8_t port = pData[0];
	RDevCmdCode ubCommand = (RDevCmdCode)pData[1];
	if(ubCommand == RDCMD_GET) {
		uint8_t pubResp[3] = {pData[0], pData[1], ubRange};
		SendCmdResp(pubResp, 3);
		return RDERR_OK;
	} else if (ubCommand == RDCMD_SET && len > 2) {
		if (pData[2] > 0) {
			nrf_drv_gpiote_in_event_enable(RjPortGetPinNum(port, PinBlue), true);
		} else {
			nrf_drv_gpiote_in_event_disable(RjPortGetPinNum(port, PinBlue));
		}
		return RDERR_DONE;
	}
	return RDERR_NOT_SUPPORTED;
}
