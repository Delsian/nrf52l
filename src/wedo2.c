/*
 * wedo2.c
 *
 *  Created on: Dec 30, 2017
 *      Author: Eug
 */

#include "custom_service.h"

static tCustomServiceVars tWedoVars;
static ble_gatts_char_handles_t tCharBtntHandle;

const ble_gatts_attr_md_t tCccdWrRd = {
		.read_perm = {
				.sm = 1,
				.lv = 1
		},
		.write_perm = {
				.sm = 1,
				.lv = 1
		},
		.vloc = BLE_GATTS_VLOC_STACK,
		.vlen = 1
};

const tCustomChar tCharBtn = {
		.usUuid = 0x1526u,
		.ptHandle = &tCharBtntHandle,
		.pCccd = &tCccdWrRd,
		.tProps = {
				.write = 1,
				.write_wo_resp = 1
		},
};

const tCustomChar tCharPortType = {
		.usUuid = 0x1527u,
		.ptHandle = NULL,
		.pCccd = &tCccdWrRd
};

const tCustomChar* tCharSet[] = {
		&tCharBtn,
		&tCharPortType,
		NULL
};

const tCustomService tServ = {
		.pubDeviceName = "LPF2 Smart Hub 2 I/O",
		//.tUuid = {00, 00, 0x15, 0x23,      0x12, 0x12,     0xef, 0xde,     0x15, 0x23,     0x78, 0x5f, 0xea, 0xbc, 0xd1, 0x23},
		.tUuid = {0x23, 0xd1, 0xbc, 0xea, 0x5f, 0x78, /*-*/ 0x23, 0x15, /*-*/ 0xde, 0xef, /*-*/ 0x12, 0x12, /*-*/ 0x23, 0x15, 0, 0},
		.ubServiceType = BLE_GATTS_SRVC_TYPE_PRIMARY,
		.ptVars = &tWedoVars,
		.ptChars = (tCustomChar**) tCharSet
};

