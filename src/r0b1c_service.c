/*
 * r0b1c_service.c
 *
 *  Created on: 15.02.18
 *      Author: ekrashtan
 */

#include <stdint.h>
#include "custom_service.h"
#include "control.h"
#include "nrf_log.h"
#include "r0b1c_cmd.h"

extern tCharVars tCharCmdHandle;
extern tCharVars tCharProgHandle;

static tCustomServiceVars tR0b1cDevice;
static tCharVars tCharBattHandle;
static tCharVars tCharBtnHandle;
static tCharVars tCharPwrOffHandle;
static tCharVars tCharOverloadHandle;

static void OnPwrWriteEvt(ble_evt_t const * p_ble_evt)
{
	const ControlEvent PwrOffEvt = {
			.type = CE_PWR_OFF
	};
	ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
	const uint32_t* ulData = (uint32_t*)p_evt_write->data;
	if (p_evt_write->len == 4 && *ulData == 0xBADF00D)
	{
		NRF_LOG_DEBUG("Pwr %x", p_evt_write->handle);
		ControlPost(&PwrOffEvt);
	}
}


void SendButtonNotification(const ControlEvent* pEvt)
{
	if (tCharBtnHandle.notif)
		CustomServiceSend(tCharBtnHandle.hval, (uint8_t *)&(pEvt->b), 1);
}

static void InitComplete()
{
	CmdInitComplete();
	ControlRegisterCb(CE_BUTTON, SendButtonNotification);
}

const tCustomService tServDev = {
		.tUuid = {{0x11, 0x5F, 0x80, 0x34, 0xE8, 0x96, /*-*/ 0x4F, 0x88, /*-*/ 0xD3, 0x4D, /*-*/ 0x19, 0xD0, /*-*/ 0x3A, 0x97, 0xAE, 0x60}},
		.ubServiceType = BLE_GATTS_SRVC_TYPE_PRIMARY,
		.ptVars = &tR0b1cDevice,
		.ptChars = {
				{ 0x973Bu, "Prog", 			CCM_WRITENOTIFY, 	&tCharProgHandle	, OnProgWriteEvt, NULL },
				{ 0x973Cu, "Cmd", 			CCM_READWRITENOTIFY,&tCharCmdHandle		, OnCmdWriteEvt	, NULL },
				{ 0x973Du, "BattLow", 		CCM_NOTIFY, 		&tCharBattHandle	, NULL			, NULL },
				{ 0x973Eu, "Btn",			CCM_READNOTIFY,		&tCharBtnHandle		, NULL			, NULL },
				{ 0x973Fu, "PwrOff", 		CCM_WRITE, 			&tCharPwrOffHandle  , OnPwrWriteEvt	, NULL },
				{ 0x9740u, "Overload", 		CCM_NOTIFY, 		&tCharOverloadHandle, NULL			, NULL },
				{ 0 }
		},
};

const tDevDescription gtServices = {
		.pubDeviceName = "r0b1c",
		.initCompl = &InitComplete,
		.tServices = {
				&tServDev,
				NULL
		}
};

void SendBatteryNotification(const uint8_t mask)
{
	if (tCharBattHandle.notif)
		CustomServiceSend(tCharBattHandle.hval, &mask, 1);
}

void SendOverloadNotification(uint8_t *pval)
{
	if (tCharOverloadHandle.notif)
		CustomServiceSend(tCharOverloadHandle.hval, pval, 1);
}

