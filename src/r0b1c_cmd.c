/*
 * r0b1c_cmd.c
 *
 *  Created on: 15.02.18
 *      Author: ekrashtan
 */

#include <string.h>
#include "custom_service.h"
#include "nrf_log.h"
#include "r0b1c_cmd.h"
#include "rj_port.h"

#define PORT_KEEP_CODE (0xFE)

tCharVars tCharCmdHandle;
tCharVars tCharPortHandle;

static RDevType ptConnectedDevs[TOTAL_RJ_PORTS];

void OnPortWriteEvt(ble_evt_t const * p_ble_evt)
{
	ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
	const uint8_t* ubData =  p_evt_write->data;
	if (p_evt_write->len == 4)
	{
		for (int i = 0; i < TOTAL_RJ_PORTS; i++)
		{
			if (ubData[i] == PORT_KEEP_CODE) continue;
			if (ubData[i] != ptConnectedDevs[i])
			{
				if (RDeviceChange(i, ubData[i]) == RDERR_OK)
				{
					ptConnectedDevs[i] = ubData[i];
					NRF_LOG_DEBUG("Port %d set dev %x", i, ubData[i]);
				}
			}
		}
		ret_code_t err = CustomServiceValueSet(tCharPortHandle.hval, ptConnectedDevs, 4);
		if (err != NRF_SUCCESS)
			NRF_LOG_DEBUG("Port wr err %x", err);
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
		if (tCmd == RDCMD_RESET) {
			// Like re-insert the same device
			RDeviceChange(port, ptConnectedDevs[port]);
		} else if (tCmd == RDCMD_ID) {
			if (port < TOTAL_RJ_PORTS) {
				uint8_t pubResp[3] = {port, tCmd, ptConnectedDevs[port]};
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

	memset(ptConnectedDevs, 0, sizeof(ptConnectedDevs));
	CustomServiceValueSet(tCharPortHandle.hval, ptConnectedDevs, 4);
}
