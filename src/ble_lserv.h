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
#include "nrf_ringbuf.h"

#define SERVICE_UUID { 0x24, 0x38, 0xEE, 0x58,    0x22, 0x06c,    0x46, 0x46,    0xad, 0xc7,    0x98, 0x94, 0xd2, 0x46, 0xe3, 0x86 }
#define BLE_UUID_LSERV_SERVICE 0xEE58
#define BLE_UUID_LSERV_RX_CHARACTERISTIC 0xEE59
#define BLE_UUID_LSERV_TX_CHARACTERISTIC 0xEE5A

#define OPCODE_LENGTH 1
#define HANDLE_LENGTH 2

/**@brief   Maximum length of data (in bytes) that can be transmitted to the peer by the service module. */
#if defined(NRF_SDH_BLE_GATT_MAX_MTU_SIZE) && (NRF_SDH_BLE_GATT_MAX_MTU_SIZE != 0)
    #define BLE_LSERV_MAX_DATA_LEN (NRF_SDH_BLE_GATT_MAX_MTU_SIZE - OPCODE_LENGTH - HANDLE_LENGTH)
#else
    #define BLE_LSERV_MAX_DATA_LEN (BLE_GATT_MTU_SIZE_DEFAULT - OPCODE_LENGTH - HANDLE_LENGTH)
    #warning NRF_SDH_BLE_GATT_MAX_MTU_SIZE is not defined.
#endif

typedef enum
{
    BLE_LSERV_EVT_RX_DATA,           /**< Data received. */
    BLE_LSERV_EVT_TX_RDY
} ble_lserv_evt_type_t;

typedef struct ble_lserv_s ble_lserv_t;

typedef struct
{
    ble_lserv_evt_type_t type;           /**< Event type. */
    ble_lserv_t * p_lserv;                 /**< A pointer to the instance. */
    union
    {
        bool b_data;
    } params;
} ble_lserv_evt_t;

typedef void (*ble_lserv_data_handler_t) (ble_lserv_evt_t * p_evt);

struct ble_lserv_s
{
    uint8_t                  uuid_type;               /**< UUID type for Service Base UUID. */
    uint16_t                 service_handle;          /**< Handle of Service (as provided by the SoftDevice). */
    ble_gatts_char_handles_t tx_handles;              /**< Handles related to the TX characteristic (as provided by the SoftDevice). */
    ble_gatts_char_handles_t rx_handles;              /**< Handles related to the RX characteristic (as provided by the SoftDevice). */
    uint16_t                 conn_handle;             /**< Handle of the current connection (as provided by the SoftDevice). BLE_CONN_HANDLE_INVALID if not in a connection. */
    bool                     is_notification_enabled; /**< Variable to indicate if the peer has enabled notification of the RX characteristic.*/
    nrf_ringbuf_t const * 	 p_tx_ringbuf;
};

// Set uuid in advdata
ret_code_t ble_lserv_init();
size_t ble_lserv_send(uint8_t* p_data, size_t* p_len);

#endif /* BLE_LSERV_H_ */
