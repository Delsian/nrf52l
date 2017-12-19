/*
 * ble_serv.c
 *
 *  Created on: 18 груд. 2017 р.
 *      Author: ekrashtan
 */

#include "bluetooth.h"
#include "ble_lserv.h"

#define SERVICE_UUID { 0x00, 0x00, 0xdf, 0xb0,    0x00, 0x00,    0x10, 0x00,    0x80, 0x00,    0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb }
#define CHAR1_UUID { 0x00, 0x00, 0xdf, 0xb1,    0x00, 0x00,    0x10, 0x00,    0x80, 0x00,    0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb }

const uint8_t char1_str[] = "MyChar1";
static uint16_t tServH;
static nrf_sdh_ble_evt_observer_t tObs;

void ble_lserv_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
    if ((p_context == NULL) || (p_ble_evt == NULL))
    {
        return;
    }

}

ret_code_t ble_lserv_init(void)
{
	ret_code_t err_code;
	//const ble_uuid128_t u128 = SERVICE_UUID;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    ble_gatts_char_handles_t char_handles;

	//err_code = sd_ble_uuid_vs_add(&u128, &(ble_uuid.type));
	//APP_ERROR_CHECK(err_code);
    ble_uuid.type = BLE_UUID_TYPE_BLE;
    ble_uuid.uuid = BLE_UUID_LSERV_SERVICE;

	// Add the service.
	err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
	                                    &ble_uuid,
	                                    &tServH);
	APP_ERROR_CHECK(err_code);

	// Add 1st Characteristic.
	memset(&char_md, 0, sizeof(char_md));

	char_md.char_props.write         = 1;
	char_md.char_props.write_wo_resp = 1;
	char_md.p_char_user_desc         = char1_str;
	char_md.char_user_desc_max_size  = sizeof(char1_str)+1;
	char_md.p_char_pf                = NULL;
	char_md.p_user_desc_md           = NULL;
	char_md.p_cccd_md                = NULL;
	char_md.p_sccd_md                = NULL;

	ble_uuid.uuid = 0xDFB1;
	memset(&attr_char_value, 0, sizeof(attr_char_value));

	memset(&attr_md, 0, sizeof(attr_md));

	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

	attr_md.vloc    = BLE_GATTS_VLOC_STACK;
	attr_md.rd_auth = 0;
	attr_md.wr_auth = 0;
	attr_md.vlen    = 1;

	attr_char_value.p_uuid    = &ble_uuid;
	attr_char_value.p_attr_md = &attr_md;
	attr_char_value.init_len  = 1;
	attr_char_value.init_offs = 0;
	attr_char_value.max_len   = 20;

	sd_ble_gatts_characteristic_add(tServH, &char_md, &attr_char_value, &char_handles);
	APP_ERROR_CHECK(err_code);

	return err_code;
}
