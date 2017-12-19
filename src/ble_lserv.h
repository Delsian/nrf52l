/*
 * ble_serv.h
 *
 *  Created on: 18 груд. 2017 р.
 *      Author: ekrashtan
 */

#ifndef BLE_LSERV_H_
#define BLE_LSERV_H_

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"

struct ble_lserv_s
{
    uint8_t                  uuid_type;               /**< UUID type for Nordic UART Service Base UUID. */
    uint16_t                 service_handle;          /**< Handle of Nordic UART Service (as provided by the SoftDevice). */
    ble_gatts_char_handles_t tx_handles;              /**< Handles related to the TX characteristic (as provided by the SoftDevice). */
    ble_gatts_char_handles_t rx_handles;              /**< Handles related to the RX characteristic (as provided by the SoftDevice). */
    uint16_t                 conn_handle;             /**< Handle of the current connection (as provided by the SoftDevice). BLE_CONN_HANDLE_INVALID if not in a connection. */
    bool                     is_notification_enabled; /**< Variable to indicate if the peer has enabled notification of the RX characteristic.*/
    //ble_nus_data_handler_t   data_handler;            /**< Event handler to be called for handling received data. */
};

typedef struct ble_lserv_s ble_lserv_t;

#define BLE_UUID_LSERV_SERVICE 0xDFB0

#define BLE_LSERV_DEF(_name)                                                                          \
static ble_lserv_t _name;                                                                             \
NRF_SDH_BLE_OBSERVER(_name ## _obs, 2, ble_lserv_on_ble_evt, &_name)

// Set uuid in advdata
void ble_lserv_adv_init(ble_uuid_t* uuid);
ret_code_t ble_lserv_init(void);
void ble_lserv_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);

#endif /* BLE_LSERV_H_ */
