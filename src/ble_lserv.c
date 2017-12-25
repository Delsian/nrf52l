/*
 * ble_lserv.c
 * Local service implementation
 */

#include "bluetooth.h"
#include "sdk_macros.h"
#include "ble_lserv.h"
#include "app_scheduler.h"

const uint8_t char1_str[] = "MyØÔÖ1";
ble_lserv_t m_lserv;
NRF_RINGBUF_DEF(tx_ringbuf, 64);

static void tx_put_scheduler(void * p_event_data, uint16_t event_size)
{
    UNUSED_PARAMETER(p_event_data);
    UNUSED_PARAMETER(event_size);

    uint8_t * p_out_data;
    size_t    out_data_len = 20; // ToDo: nrf_ble_gatt_eff_mtu_get(p_instance->p_gatt, p_instance->p_cb->conn_handle) - OVERHEAD_LENGTH;

    ret_code_t err_code = nrf_ringbuf_get(m_lserv.p_tx_ringbuf,
										 &p_out_data,
										 &out_data_len,
										 true);
	if (err_code == NRF_SUCCESS) {
		if (out_data_len > 0) {
			ble_gatts_hvx_params_t hvx_params;
			uint16_t len = out_data_len;
			hvx_params.handle = m_lserv.tx_handles.value_handle;
			hvx_params.p_data = p_out_data;
			hvx_params.p_len  = &len;
			hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;

			sd_ble_gatts_hvx(m_lserv.conn_handle, &hvx_params);
			if ((err_code == NRF_ERROR_BUSY) || (err_code == NRF_ERROR_RESOURCES))
			{
			   out_data_len = 0;
			}
			err_code = nrf_ringbuf_free(m_lserv.p_tx_ringbuf, out_data_len);
			ASSERT(err_code == NRF_SUCCESS);
			printf("tx %d bytes\n", out_data_len);
		}
	}
}

static void ble_lserv_rx_evt(ble_evt_t const * p_evt)
{
	ble_gatts_evt_write_t const * p_evt_write = &p_evt->evt.gatts_evt.params.write;

	if (p_evt_write->handle == m_lserv.tx_handles.cccd_handle)
	{
		m_lserv.is_notification_enabled = ble_srv_is_notification_enabled(p_evt_write->data);
		printf("Notification %s\n", m_lserv.is_notification_enabled?"On":"Off");
	}
	else if (p_evt_write->handle == m_lserv.rx_handles.value_handle)
	{
		printf("rx %s\n", p_evt_write->data);
		uint8_t test[] = "34635635635663563tatatatatattataahdhsrhstrhsrthsdhsftjhfjtrjrtjrtjrjrtjrjtjrtjjrtjrjrjrjtjrtjrttjtjrtjtjrtjrtjr";
		size_t ll = strlen(test);
		ble_lserv_send(test, &ll);
	}
}

static void on_connect(ble_lserv_t * p_lserv, ble_evt_t const * p_ble_evt)
{
    p_lserv->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}

static void on_disconnect(ble_lserv_t * p_lserv, ble_evt_t const * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_lserv->conn_handle = BLE_CONN_HANDLE_INVALID;
}

// For incoming events
static uint32_t rx_char_add()
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.write         = 1;
    char_md.char_props.write_wo_resp = 1;
    char_md.p_char_user_desc         = char1_str;
    char_md.p_char_pf                = NULL;
    char_md.p_user_desc_md           = NULL;
    char_md.p_cccd_md                = NULL;
    char_md.p_sccd_md                = NULL;

    ble_uuid.type = m_lserv.uuid_type;
    ble_uuid.uuid = BLE_UUID_LSERV_RX_CHARACTERISTIC;

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

	return sd_ble_gatts_characteristic_add(m_lserv.service_handle,
	                                           &char_md,
	                                           &attr_char_value,
	                                           &m_lserv.rx_handles);
}

static uint32_t tx_char_add()
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&cccd_md, 0, sizeof(cccd_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);

    cccd_md.vloc = BLE_GATTS_VLOC_STACK;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.notify = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md;
    char_md.p_sccd_md         = NULL;

    ble_uuid.type = m_lserv.uuid_type;
    ble_uuid.uuid = BLE_UUID_LSERV_TX_CHARACTERISTIC;

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
    attr_char_value.init_len  = sizeof(uint8_t);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = BLE_LSERV_MAX_DATA_LEN;

    return sd_ble_gatts_characteristic_add(m_lserv.service_handle,
                                           &char_md,
                                           &attr_char_value,
                                           &m_lserv.tx_handles);
    /**@snippet [Adding proprietary characteristic to the SoftDevice] */
}

static void ble_lserv_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
    if ((p_context == NULL) || (p_ble_evt == NULL))
    {
        return;
    }

    //printf("evt %d\n", p_ble_evt->header.evt_id);
    ble_lserv_t * p_serv = (ble_lserv_t *)p_context;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
        	on_connect(p_serv, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_serv, p_ble_evt);
            break;

        case BLE_GATTS_EVT_WRITE:
        	ble_lserv_rx_evt(p_ble_evt);
            break;

        case BLE_GATTS_EVT_HVN_TX_COMPLETE:
        {
            //notify with empty data that some tx was completed.
        	app_sched_event_put(NULL, 0, tx_put_scheduler);
            break;
        }
        default:
            // No implementation needed.
            break;
    }

}

NRF_SDH_BLE_OBSERVER(m_lserv_obs, 2, ble_lserv_on_ble_evt, &m_lserv);

size_t ble_lserv_send(uint8_t* p_data, size_t* p_len)
{
	if(m_lserv.is_notification_enabled) {
		ret_code_t err_code = nrf_ringbuf_cpy_put(m_lserv.p_tx_ringbuf, p_data, p_len);
		if (err_code == NRF_SUCCESS) {
			app_sched_event_put(NULL, 0, tx_put_scheduler);
			return *p_len;
		}
	}
	return 0;
}

ret_code_t ble_lserv_init()
{
	ret_code_t err_code;
	ble_uuid128_t u128 = SERVICE_UUID;
	ble_uuid_t          ble_uuid;

    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_gatts_attr_md_t attr_md;
    ble_gatts_char_handles_t char_handles;

    m_lserv.conn_handle             = BLE_CONN_HANDLE_INVALID;
    m_lserv.is_notification_enabled = false;

	err_code = sd_ble_uuid_vs_add(&u128, &m_lserv.uuid_type);
	VERIFY_SUCCESS(err_code);

    ble_uuid.type = m_lserv.uuid_type;
    ble_uuid.uuid = BLE_UUID_LSERV_SERVICE;

	// Add the service.
	err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
	                                    &ble_uuid,
	                                    &m_lserv.service_handle);
	VERIFY_SUCCESS(err_code);

	m_lserv.p_tx_ringbuf = &tx_ringbuf;
	nrf_ringbuf_init(m_lserv.p_tx_ringbuf);

    err_code = rx_char_add();
    VERIFY_SUCCESS(err_code);
    err_code = tx_char_add();
    VERIFY_SUCCESS(err_code);

    return NRF_SUCCESS;
}
