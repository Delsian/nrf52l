/*
 * r0b1c_cmd.c
 *
 *  Created on: 15.02.18
 *      Author: ekrashtan
 */


#include "custom_service.h"
#include "nrf_log.h"
#include "r0b1c_device.h"
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
		if (tCharPortHandle.notif)
		{
			CustomServiceSend(tCharPortHandle.hcccd, ptConnectedDevs, 4);
		}
	}
}


void OnPortNotifyEvt(ble_evt_t const * p_ble_evt)
{
	NRF_LOG_DEBUG("PortNotify %d", tCharPortHandle.notif);
	if (tCharPortHandle.notif)
	{
		CustomServiceSend(tCharPortHandle.hcccd, ptConnectedDevs, 4);
	}
}

void OnCmdWriteEvt(ble_evt_t const * p_ble_evt)
{
	ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
	const uint8_t* ubData =  p_evt_write->data;
	if (p_evt_write->len >= 2)
	{
		NRF_LOG_DEBUG("CmdW %x\n", p_evt_write->handle);
	}
}


