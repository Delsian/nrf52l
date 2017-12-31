/*
 * custom_service.h
 *
 *  Created on: Dec 30, 2017
 *      Author: Eug
 */

#ifndef CUSTOM_SERVICE_H_
#define CUSTOM_SERVICE_H_

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "bluetooth.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"

typedef struct _CustomServiceVars {
	nrf_ble_gatt_t *		 ptGatt;
	ble_uuid_t               tUuid;                    /**< UUID type for Service Base UUID. */
	uint16_t                 usServiceHandle;          /**< Handle of Service (as provided by the SoftDevice). */
	uint16_t                 usConnHandle;             /**< Handle of the current connection (as provided by the SoftDevice). BLE_CONN_HANDLE_INVALID if not in a connection. */
} tCustomServiceVars;

typedef struct _CustomChar {
	uint16_t usUuid;
	ble_gatts_char_handles_t* ptHandle;
	ble_gatt_char_props_t tProps;
	const ble_gatts_attr_md_t* pCccd;
} tCustomChar;

typedef struct _CustomService {
	uint8_t*				pubDeviceName;
	ble_uuid128_t 			tUuid;
	uint8_t 				ubServiceType;
	tCustomServiceVars* 	ptVars;
	tCustomChar** 			ptChars;
} tCustomService;

// Define this structure in external module
extern const tCustomService tServ;

#endif /* CUSTOM_SERVICE_H_ */

ret_code_t CustomServiceInit(const tCustomService* itServ);
