/*
 * wedo2.c
 *
 *  Created on: Dec 30, 2017
 *      Author: Eug
 */

#include "custom_service.h"

static tCustomServiceVars tWedoDevice;
static ble_gatts_char_handles_t tCharBtnHandle;
static ble_gatts_char_handles_t tCharPortHandle;
static ble_gatts_char_handles_t tCharVoltHandle;
static ble_gatts_char_handles_t tCharCurrHandle;
static ble_gatts_char_handles_t tCharSignalHandle;
static ble_gatts_char_handles_t tCharTurnOffHandle;
static ble_gatts_char_handles_t tCharVccCtlHandle;
static tCustomServiceVars tWedoIo;
static ble_gatts_char_handles_t tCharSnsHandle;
static ble_gatts_char_handles_t tCharValHandle;
static ble_gatts_char_handles_t tCharInHandle;
static ble_gatts_char_handles_t tCharOutHandle;

static void OnWriteEvt(ble_evt_t const * p_ble_evt)
{
	ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
	printf("HndW %x\n", p_evt_write->handle);
}


static void OnReadEvt(ble_evt_t const * p_ble_evt)
{
	printf("HndR %x\n", p_ble_evt->evt.gatts_evt.conn_handle);
}

const tCustomService tServDev = {
		.tUuid = {0x23, 0xd1, 0xbc, 0xea, 0x5f, 0x78, /*-*/ 0x23, 0x15, /*-*/ 0xde, 0xef, /*-*/ 0x12, 0x12, /*-*/ 0x23, 0x15, 0, 0},
		.ubServiceType = BLE_GATTS_SRVC_TYPE_PRIMARY,
		.ptVars = &tWedoDevice,
		.wrEvt = &OnWriteEvt,
		.rdEvt = &OnReadEvt,
		.ptChars = {
				{ 0x1526u, "btn", 			CCM_READNOTIFY, 	&tCharBtnHandle		}, // 11 00	17	00001526 1212 efde 1523 785feabcd123	Button Char
				{ 0x1527u, "porttype", 		CCM_NOTIFY, 		&tCharPortHandle	}, // 15 00	21	00001527 1212 efde 1523 785feabcd123	Port Type Char
				{ 0x1528u, "voltage", 		CCM_READNOTIFY, 	&tCharVoltHandle	}, // 19 00	25	00001528 1212 efde 1523 785feabcd123	Low Voltage alert
				{ 0x1529u, "current",		CCM_READNOTIFY,		&tCharCurrHandle	}, // 1D 00	29	00001529 1212 efde 1523 785feabcd123	High Current alert
				{ 0x152Au, "lowsignal", 	CCM_READNOTIFY, 	&tCharSignalHandle  }, // 21 00	33	0000152a 1212 efde 1523 785feabcd123	Low Signal alert
				{ 0x152Bu, "turnoff", 		CCM_WRITE, 			&tCharTurnOffHandle }, // 25 00	37	0000152b 1212 efde 1523 785feabcd123	Turn off device
				{ 0x152Cu, "vccctl", 		CCM_READWRITE, 		&tCharVccCtlHandle  }, // 28 00	40	0000152c 1212 efde 1523 785feabcd123	Vcc port control
				{ 0 }
		},
};

const tCustomService tServIo = {
		.tUuid = {0x23, 0xd1, 0xbc, 0xea, 0x5f, 0x78, /*-*/ 0x23, 0x15, /*-*/ 0xde, 0xef, /*-*/ 0x12, 0x12, /*-*/ 0x0E, 0x4F, 0, 0},
		.ubServiceType = BLE_GATTS_SRVC_TYPE_PRIMARY,
		.ptVars = &tWedoIo,
		.wrEvt = &OnWriteEvt,
		.rdEvt = &OnReadEvt,
		.ptChars = {
				{ 0x1560u, "sensor", 		CCM_READNOTIFY, 	&tCharSnsHandle		}, // 32 00	50	00001560 1212 efde 1523 785feabcd123	Sensor Value
				{ 0x1561u, "value", 		CCM_READNOTIFY, 	&tCharValHandle		}, // 36 00	54	00001561 1212 efde 1523 785feabcd123	Value format
				{ 0x1563u, "incmd", 		CCM_READWRITE, 		&tCharInHandle      }, // 3a 00	58	00001563 1212 efde 1523 785feabcd123	Input Command
				{ 0x1565u, "outcmd", 		CCM_READWRITE, 		&tCharOutHandle     }, // 3d 00	61	00001565 1212 efde 1523 785feabcd123	Output Command
				{ 0 }
		},
};

const tDevDescription gtServices = {
		.pubDeviceName = "LPF2 Smart Hub 2 I/O",
		.tServices = {
				&tServDev,
				&tServIo,
				NULL
		}
};
