/*
 * r0b1c_cmd.c
 *
 *  Created on: 15.02.18
 *      Author: ekrashtan
 */

#include <string.h>
#include "custom_service.h"
#include "nrf_log.h"
#include "control.h"
#include "r0b1c_cmd.h"
#include "r0b1c_device.h"
#include "rj_port.h"

tCharVars tCharCmdHandle;
tCharVars tCharProgHandle;

static bool eProgWriteInProgress;

void OnProgWriteEvt(ble_evt_t const * p_ble_evt)
{
	static uint16_t usProgLen;

	ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
	const uint8_t* ubData =  p_evt_write->data;
	if (!eProgWriteInProgress && p_evt_write->len > 0) {
		switch (ubData[0]) {
		case 0: // Reset
			break;
		case 1: // write
			usProgLen = ubData[1] + ubData[2]*256;
			eProgWriteInProgress = true;
			NRF_LOG_DEBUG("WrProg %d bytes", usProgLen);
			break;
		default:
			break;
		}
	} else {
		// Next block
		NRF_LOG_DEBUG("c%d: %c", p_evt_write->len, ubData[0]);

		usProgLen -= p_evt_write->len;
		if (usProgLen == 0) eProgWriteInProgress = false;
	}
}

static void OnProgWriteCb(const ControlEvent* pEvt)
{
	if (pEvt->type == CE_BT_CONN && *(pEvt->ptr16) == BLE_CONN_HANDLE_INVALID ) {
		eProgWriteInProgress = false;
	}
}

void OnCmdWriteEvt(ble_evt_t const * p_ble_evt)
{
	RDevErrCode tErr = RDERR_INCOMPLETE;
	ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
	const uint8_t* ubData =  p_evt_write->data;
	if (p_evt_write->len >= 2)
	{
		uint8_t port = ubData[0];
		RDevCmdCode tCmd = (RDevCmdCode)ubData[1];
		NRF_LOG_DEBUG("Cmd %x to %x", tCmd, port);
		if (tCmd == RDCMD_CONFIG && p_evt_write->len == 3) {
			if (port < TOTAL_RJ_PORTS) {
				tErr = RDeviceChange(port, ubData[2]);
			} else {
				tErr = RDERR_NOT_SUPPORTED;
			}
		} else if (tCmd == RDCMD_ID) {
			if (port < TOTAL_RJ_PORTS) {
				uint8_t pubResp[3] = {port, tCmd, GetDevId(port)};
				SendCmdResp(pubResp, 3);
				tErr = RDERR_OK;
			} else {
				tErr = RDERR_NOT_SUPPORTED;
			}
		} else {
			tErr = RDeviceCmd(ubData, p_evt_write->len);
		}
	}

	if (tErr != RDERR_OK)
		CmdErrWrite(tErr);
}

void SendCmdResp(uint8_t* ipubData, uint8_t iubLen)
{
	NRF_LOG_DEBUG("TxResp %x %x len %d", ipubData[0], ipubData[1], iubLen);
	CustomServiceValueSet(tCharCmdHandle.hval, ipubData, iubLen);
}

void SendCmdNotif(uint8_t* ipubData, uint8_t iubLen)
{
	if (tCharCmdHandle.notif)
		CustomServiceSend(tCharCmdHandle.hval, ipubData, iubLen);
}

void CmdErrWrite(RDevErrCode itErrCode)
{
	uint8_t pubErr[2] = {0xFE, (uint8_t)itErrCode};
	NRF_LOG_DEBUG("TxErr %x", itErrCode);
	CustomServiceValueSet(tCharCmdHandle.hval, pubErr, 2);
}

void CmdInitComplete()
{
	// ToDo: move it here from main()
    //RDeviceInit();

	ControlRegisterCb(CE_BT_CONN, OnProgWriteCb);
}
