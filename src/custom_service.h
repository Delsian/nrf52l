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
#include "nrf_mtx.h"

#define DEVICE_NAME_LEN 20

typedef struct _CustomServiceVars {
	nrf_ble_gatt_t *		 ptGatt;
	ble_uuid_t               tUuid;                    /**< UUID type for Service Base UUID. */
	uint16_t                 usServiceHandle;          /**< Handle of Service (as provided by the SoftDevice). */
	uint16_t                 usConnHandle;             /**< Handle of the current connection (as provided by the SoftDevice). BLE_CONN_HANDLE_INVALID if not in a connection. */
} tCustomServiceVars;

typedef enum {
	CCM_WRITE,
	CCM_READNOTIFY,
	CCM_READWRITE,
	CCM_WRITENOTIFY,
	CCM_READWRITENOTIFY,
	CCM_NOTIFY
} CustomCharMode;

typedef struct _CharVars {
	uint16_t 	hval;
	uint16_t 	hcccd;
	bool		notif;
} tCharVars;

typedef void (CustEventReceiver)(ble_evt_t const *);
typedef void (CustServInitComplete)(void);

typedef struct _CustomChar {
	uint16_t			usUuid;
	uint8_t*			ubName;
	CustomCharMode		tMode;
	tCharVars* 			ptHandle;
	CustEventReceiver*  wrEvt; /* Call this function on write event */
	CustEventReceiver*	notifyEvt; /* Call this function on notification enable/disable */
} tCustomChar;

typedef struct _CustomService {
	ble_uuid128_t 			tUuid;
	uint8_t 				ubServiceType;
	tCustomServiceVars* 	ptVars;
	tCustomChar 			ptChars[];
} tCustomService;

typedef struct _DeviceDescription {
	const uint8_t*			pubDeviceName;
	CustServInitComplete*	initCompl; // Custom service init complete callback
	const tCustomService*	tServices[2]; // Up to two Custom UUIDS
} tDevDescription;

// Define this structure in external module
extern const tDevDescription gtServices;

#endif /* CUSTOM_SERVICE_H_ */

ret_code_t CustomServiceInit(const tCustomService* itServ);
uint16_t GetConnectionHandle(void);
void CustomServiceSend(uint16_t iusChar, const uint8_t *pubData, uint16_t iusLen);
ret_code_t CustomServiceValueSet(uint16_t iusChar, uint8_t* ipubData, uint8_t iubLen);
void CustGetDeviceName(uint8_t* opubBuf);
