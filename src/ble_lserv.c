/*
 * ble_lserv.c
 * Local service implementation
 */

#include "bluetooth.h"
#include "sdk_macros.h"
#include "ble_lserv.h"

const uint8_t char1_str[] = "MyChar1";

void ble_lserv_evt(ble_lserv_evt_t * p_evt)
{

}

static uint32_t Ist_char_add(ble_lserv_t * p_lserv)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.write         = 1;
    char_md.char_props.write_wo_resp = 1;
    char_md.p_char_user_desc         = NULL;
    char_md.p_char_pf                = NULL;
    char_md.p_user_desc_md           = NULL;
    char_md.p_cccd_md                = NULL;
    char_md.p_sccd_md                = NULL;

    ble_uuid.type = p_lserv->uuid_type;
    ble_uuid.uuid = BLE_UUID_LSERV_1ST_CHARACTERISTIC;

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    attr_md.vloc    = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth = 0;
    attr_md.wr_auth = 0;
    attr_md.vlen    = 1;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = 1;
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = BLE_LSERV_MAX_DATA_LEN;

	return sd_ble_gatts_characteristic_add(p_lserv->service_handle,
	                                           &char_md,
	                                           &attr_char_value,
	                                           &p_lserv->rx_handles);
}

static void __attribute__ ((section(".sdh_ble_observers"))) __attribute__((used)) ble_lserv_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
    if ((p_context == NULL) || (p_ble_evt == NULL))
    {
        return;
    }

}

ret_code_t ble_lserv_init(ble_lserv_t * p_lserv)
{
	ret_code_t err_code;
	ble_uuid128_t u128 = SERVICE_UUID;
	ble_uuid_t          ble_uuid;

    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_gatts_attr_md_t attr_md;
    ble_gatts_char_handles_t char_handles;

    VERIFY_PARAM_NOT_NULL(p_lserv);

    p_lserv->conn_handle             = BLE_CONN_HANDLE_INVALID;
    p_lserv->data_handler            = ble_lserv_evt;
    p_lserv->is_notification_enabled = false;

	err_code = sd_ble_uuid_vs_add(&u128, &p_lserv->uuid_type);
	VERIFY_SUCCESS(err_code);

    ble_uuid.type = p_lserv->uuid_type;
    ble_uuid.uuid = BLE_UUID_LSERV_SERVICE;

	// Add the service.
	err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
	                                    &ble_uuid,
	                                    &p_lserv->service_handle);
	VERIFY_SUCCESS(err_code);

    err_code = Ist_char_add(p_lserv);
    VERIFY_SUCCESS(err_code);

    return NRF_SUCCESS;
}
