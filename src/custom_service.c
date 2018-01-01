/*
 * custom_service.c
 *
 *  Created on: Dec 30, 2017
 *      Author: Eug
 */

#include "custom_service.h"
#include "sdk_macros.h"

static tCustomServiceVars* ptCustVar;
static CustEventReceiver* RdHndlr;
static CustEventReceiver* WrHndlr;

static void ble_custom_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
    if (p_ble_evt == NULL)
    {
        return;
    }

    printf("evt %d\n", p_ble_evt->header.evt_id);

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
        	ptCustVar->usConnHandle = p_ble_evt->evt.gap_evt.conn_handle;
            break;

        case BLE_GAP_EVT_DISCONNECTED:
        	ptCustVar->usConnHandle = BLE_CONN_HANDLE_INVALID;
            break;

        case BLE_GATTS_EVT_WRITE:
        	if(WrHndlr) {
        		(*WrHndlr)(p_ble_evt);
        	}
            break;

        case BLE_GATTS_EVT_HVN_TX_COMPLETE:
            //notify with empty data that some tx was completed.
        	if(RdHndlr) {
				(*RdHndlr)(p_ble_evt);
			}
            break;
        default:
            // No implementation needed.
            break;
    }

}

NRF_SDH_BLE_OBSERVER(cust_obs, 2, ble_custom_on_ble_evt, NULL);

ret_code_t CustomServiceInit(const tCustomService* itServ)
{
	ret_code_t err_code;
	ptCustVar = itServ->ptVars;

	ptCustVar->usConnHandle = BLE_CONN_HANDLE_INVALID;

	err_code = sd_ble_uuid_vs_add(&(itServ->tUuid), &(ptCustVar->tUuid.type));
	VERIFY_SUCCESS(err_code);

	ptCustVar->tUuid.uuid = itServ->tUuid.uuid128[13];
	ptCustVar->tUuid.uuid <<= 8;
	ptCustVar->tUuid.uuid += itServ->tUuid.uuid128[12];
	// Add the service.
	err_code = sd_ble_gatts_service_add(itServ->ubServiceType,
										&(ptCustVar->tUuid),
										&(ptCustVar->usServiceHandle));
	VERIFY_SUCCESS(err_code);

	uint8_t ubChIndex = 0;
	while (itServ->ptChars[ubChIndex].usUuid)
	{
		ble_gatts_char_handles_t tNewHandle;
		ble_gatts_char_md_t char_md;
	    ble_gatts_attr_md_t cccd_md;
		ble_gatts_attr_md_t attr_md;
		ble_gatts_attr_t    attr_char_value;
		ble_uuid_t          ble_uuid;

		memset(&char_md, 0, sizeof(char_md));
		memset(&attr_char_value, 0, sizeof(attr_char_value));
		memset(&cccd_md, 0, sizeof(cccd_md));

		BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
		BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);

		cccd_md.vloc = BLE_GATTS_VLOC_STACK;
	    char_md.p_cccd_md         = &cccd_md;

	    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
	    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
	    attr_md.vloc    = BLE_GATTS_VLOC_STACK;
	    attr_md.vlen    = 1;

		switch(itServ->ptChars[ubChIndex].tMode) {
		case CCM_WRITE:
			char_md.char_props.write = 1;
			break;
		case CCM_READNOTIFY:
		    char_md.char_props.notify = 1;
		    char_md.char_props.read = 1;
			break;
		case CCM_NOTIFY:
		    char_md.char_props.notify = 1;
			break;
		case CCM_READWRITE:
			char_md.char_props.write = 1;
			char_md.char_props.read = 1;
			break;
		}

		ble_uuid.type = ptCustVar->tUuid.type;
		ble_uuid.uuid = itServ->ptChars[ubChIndex].usUuid;

		attr_char_value.p_attr_md = &attr_md;
		attr_char_value.p_uuid    = &ble_uuid;
		attr_char_value.init_len  = sizeof(uint8_t);
		attr_char_value.max_len   = 20;

		printf("Char %x\n", itServ->ptChars[ubChIndex].usUuid);
		err_code = sd_ble_gatts_characteristic_add(ptCustVar->usServiceHandle,
		                                           &char_md,
		                                           &attr_char_value,
		                                           &tNewHandle);
		VERIFY_SUCCESS(err_code);
		printf("Hnd %x %x %x\n", tNewHandle.value_handle, tNewHandle.user_desc_handle, tNewHandle.cccd_handle);
		if (itServ->ptChars[ubChIndex].ptHandle) {
			memcpy(itServ->ptChars[ubChIndex].ptHandle, &tNewHandle, sizeof(ble_gatts_char_handles_t));
		}
		ubChIndex++;
	}

	if (itServ->rdEvt) {
		RdHndlr = itServ->rdEvt;
	}
	if (itServ->wrEvt) {
		WrHndlr = itServ->wrEvt;
	}

    return NRF_SUCCESS;
}