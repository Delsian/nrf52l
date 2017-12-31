/*
 * custom_service.c
 *
 *  Created on: Dec 30, 2017
 *      Author: Eug
 */

#include "custom_service.h"
#include "sdk_macros.h"

static tCustomServiceVars* ptCustVar;

static void ble_custom_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
    if ((p_context == NULL) || (p_ble_evt == NULL))
    {
        return;
    }

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
        	ptCustVar->usConnHandle = p_ble_evt->evt.gap_evt.conn_handle;
            break;

        case BLE_GAP_EVT_DISCONNECTED:
        	ptCustVar->usConnHandle = BLE_CONN_HANDLE_INVALID;
            break;

        case BLE_GATTS_EVT_WRITE:
        	//ble_lserv_rx_evt(p_ble_evt);
            break;

        case BLE_GATTS_EVT_HVN_TX_COMPLETE:
        {
            //notify with empty data that some tx was completed.
        	//app_sched_event_put(NULL, 0, tx_put_scheduler);
            break;
        }
        default:
            // No implementation needed.
            break;
    }

}

NRF_SDH_BLE_OBSERVER(cust_obs, 2, ble_custom_on_ble_evt, NULL);

ret_code_t CustomServiceInit(const tCustomService* itServ)
{
	ret_code_t err_code;
	tCustomChar** ptChars = itServ->ptChars;
	ptCustVar = itServ->ptVars;

	ptCustVar->usConnHandle = BLE_CONN_HANDLE_INVALID;

	err_code = sd_ble_uuid_vs_add(&(itServ->tUuid), &(ptCustVar->tUuid.type));
	VERIFY_SUCCESS(err_code);

	ptCustVar->tUuid.uuid = itServ->tUuid.uuid128[13]<<8 + itServ->tUuid.uuid128[12];
	// Add the service.
	err_code = sd_ble_gatts_service_add(itServ->ubServiceType,
										&(ptCustVar->tUuid),
										&(ptCustVar->usServiceHandle));
	VERIFY_SUCCESS(err_code);

	while (*ptChars)
	{
		ble_gatts_char_handles_t tNewHandle;
		tCustomChar* ptNew = *ptChars;
		ble_gatts_char_md_t char_md;		ble_gatts_attr_md_t attr_md;
		ble_gatts_attr_t    attr_char_value;
		ble_uuid_t          ble_uuid;

		memset(&char_md, 0, sizeof(char_md));
		memset(&attr_char_value, 0, sizeof(attr_char_value));

		char_md.char_props = ptNew->tProps;
		char_md.p_cccd_md  = ptNew->pCccd;

		ble_uuid.type = ptCustVar->tUuid.type;
		ble_uuid.uuid = ptNew->usUuid;

		attr_char_value.p_attr_md = ptNew->pCccd;
		attr_char_value.p_uuid    = &ble_uuid;
		attr_char_value.init_len  = sizeof(uint8_t);
		attr_char_value.max_len   = 20;

		printf("Char %x\n", ptNew->usUuid);
		err_code = sd_ble_gatts_characteristic_add(ptCustVar->usServiceHandle,
		                                           &char_md,
		                                           &attr_char_value,
		                                           &tNewHandle);
		VERIFY_SUCCESS(err_code);
		if (ptNew->ptHandle) {
			memcpy(ptNew->ptHandle, &tNewHandle, sizeof(ble_gatts_char_handles_t));
		}
		ptChars++;
	}

    return NRF_SUCCESS;
}
