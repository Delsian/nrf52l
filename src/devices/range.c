/*
 * range.c
 *
 *  Created on: 12-03-18
 *      Author: ekrashtan
 */

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "rj_port.h"
#include "nrf_delay.h"
#include "app_timer.h"
#include "nrf_drv_gpiote.h"
#include "r0b1c_device.h"
#include "r0b1c_cmd.h"

static uint8_t ubPin[4];
static uint8_t ubRange[4]; // keeping range value for port
volatile bool isWaiting; // Indicates active cycle

static void RDevRangeToggle(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	static uint32_t time;

	uint8_t notif[3] = {3, RDCMD_GET, 0}; // ToDo port number!!!!!

	if(action==NRF_GPIOTE_POLARITY_TOGGLE) {

	}
}

// Yellow - trigger
// Blue - Echo
RDevErrCode RDevRangeInit(uint8_t port)
{
	RjPortSetOutput(port, PinYellow);
	RjPortPinSet(port, PinYellow, 0);

	nrf_drv_gpiote_in_config_t conf = {
			.is_watcher = false,                     \
			.hi_accuracy = true,                  \
			.pull = NRF_GPIO_PIN_PULLUP,             \
			.sense = NRF_GPIOTE_POLARITY_TOGGLE
	};
	uint32_t pin = RjPortGetPin(port, PinBlue);
	nrf_drv_gpiote_in_init(pin, &conf, &RDevRangeToggle);
	ubPin[port] = (uint8_t)pin;
	isWaiting = false;
	nrf_drv_gpiote_in_event_enable(pin, true); // ToDo move to RDCMD_SET
	return RDERR_OK;
}

RDevErrCode RDevRangeUnInit(uint8_t port)
{
	nrf_drv_gpiote_in_event_enable(RjPortGetPin(port, PinBlue), false);
	nrf_drv_gpiote_in_uninit(RjPortGetPin(port, PinBlue));
	ubPin[port] = 0xFF;
	return RDERR_OK;
}

RDevErrCode RDevRangeTick(uint8_t port, uint32_t time)
{
	if ((time & 0x7F) == 25) { // checking active port every 128th tick, use random shift from timer start
		if(((time&0x180)>>6) == port) { // equal shift for more than one sensor
			RjPortPinSet(port, PinYellow, 1);
			nrf_delay_us(10);
			RjPortPinSet(port, PinYellow, 0);
		}
	}
	return RDERR_DONE;
}

RDevErrCode RDevRangeCmd(const uint8_t* pData, uint8_t len)
{
	uint8_t port = pData[0];
	RDevCmdCode ubCommand = (RDevCmdCode)pData[1];
	if(ubCommand == RDCMD_GET) {
		uint8_t pubResp[3] = {pData[0], pData[1], ubRange[port]};
		SendCmdResp(pubResp, 3);
		return RDERR_OK;
	} else if (ubCommand == RDCMD_SET && len > 2) {
		uint32_t pin = RjPortGetPin(port, PinBlue);
		if (pData[2] > 0) {
			//tNotificationPin[port] = pin;
		} else {
			//tNotificationPin[port] = 0xFFFF;
		}
		return RDERR_DONE;
	}
	return RDERR_NOT_SUPPORTED;
}
