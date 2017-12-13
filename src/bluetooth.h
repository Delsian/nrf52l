/*
 * ble.h
 *
 *  Created on: Dec 9, 2017
 *      Author: Eug
 */

#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

#include "ble.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_gap.h"
#include "ble_conn_params.h"
#include "ble_db_discovery.h"
#include "nrf_ble_gatt.h"
#include "ble_hci.h"
#include "ble_nus.h"

#define APP_FEATURE_NOT_SUPPORTED       BLE_GATT_STATUS_ATTERR_APP_BEGIN + 2    /**< Reply when unsupported features are requested. */

void ble_stack_thread(void * arg);
uint32_t bluetooth_send(uint8_t* str, uint16_t len);

#endif /* BLUETOOTH_H_ */
