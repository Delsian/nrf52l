/*
 * ble.c
 *
 *  Created on: Dec 9, 2017
 *      Author: Eug
 */

#include "bluetooth.h"
#include "ble_bas.h"
#include "app_timer.h"
#include "app_scheduler.h"

#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_soc.h"
#include "ble_dfu.h"
#include "ble_dis.h"
#include "nrf_log.h"
#include "nrf_pwr_mgmt.h"
#include "custom_service.h"
#include "nrf_dfu_types.h"
#include "rdev_led.h"

#define APP_BLE_OBSERVER_PRIO           2
#define APP_BLE_CONN_CFG_TAG            1

#define APP_ADV_INTERVAL                64                                      /**< The advertising interval (in units of 0.625 ms; this value corresponds to 40 ms). */

NRF_BLE_GATT_DEF(m_gatt);                                                       /**< GATT module instance. */
static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;

// Used UUIDs
BLE_BAS_DEF(m_bas);
//====

const LedPatternSeq BtConnLed = {
		.repeats = 0xFF,
		.length = 2,
		.pt = { {0x2, COLOR_BLUE },
				{0x30, COLOR_BLACK }}
};

void battery_level_update(uint8_t battery_level)
{
    ret_code_t err_code;

    err_code = ble_bas_battery_level_update(&m_bas, battery_level);
    if ((err_code != NRF_SUCCESS) &&
        (err_code != NRF_ERROR_INVALID_STATE) &&
        (err_code != NRF_ERROR_RESOURCES) &&
        (err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)
       )
    {
        APP_ERROR_HANDLER(err_code);
    }
}

/**@brief Handler for shutdown preparation.
 *
 * @details During shutdown procedures, this function will be called at a 1 second interval
 *          untill the function returns true. When the function returns true, it means that the
 *          app is ready to reset to DFU mode.
 *
 * @param[in]   event   Power manager event.
 *
 * @retval  True if shutdown is allowed by this power manager handler, otherwise false.
 */
static bool app_shutdown_handler(nrf_pwr_mgmt_evt_t event)
{
    switch (event)
    {
        case NRF_PWR_MGMT_EVT_PREPARE_DFU:
        	NRF_LOG_INFO("Power management wants to reset to DFU mode");
            break;

        default:
            // Implement any of the other events available from the power management module:
            //      -NRF_PWR_MGMT_EVT_PREPARE_SYSOFF
            //      -NRF_PWR_MGMT_EVT_PREPARE_WAKEUP
            //      -NRF_PWR_MGMT_EVT_PREPARE_RESET
            return true;
    }
    return true;
}

NRF_PWR_MGMT_HANDLER_REGISTER(app_shutdown_handler, 0);

/**@brief Function for handling dfu events from the Buttonless Secure DFU service
 *
 * @param[in]   event   Event from the Buttonless Secure DFU service.
 */
static void ble_dfu_evt_handler(ble_dfu_buttonless_evt_type_t event)
{
    switch (event)
    {
        case BLE_DFU_EVT_BOOTLOADER_ENTER_PREPARE:
        	NRF_LOG_INFO("Device is preparing to enter bootloader mode");
            // YOUR_JOB: Disconnect all bonded devices that currently are connected.
            //           This is required to receive a service changed indication
            //           on bootup after a successful (or aborted) Device Firmware Update.
            break;

        case BLE_DFU_EVT_BOOTLOADER_ENTER:
            // YOUR_JOB: Write app-specific unwritten data to FLASH, control finalization of this
            //           by delaying reset by reporting false in app_shutdown_handler
        	NRF_LOG_INFO("Device will enter bootloader mode");
            break;

        case BLE_DFU_EVT_BOOTLOADER_ENTER_FAILED:
        	NRF_LOG_INFO("Request to enter bootloader mode failed asynchroneously");
            // YOUR_JOB: Take corrective measures to resolve the issue
            //           like calling APP_ERROR_CHECK to reset the device.
            break;

        case BLE_DFU_EVT_RESPONSE_SEND_ERROR:
            printf("Request to send a response to client failed\n");
            // YOUR_JOB: Take corrective measures to resolve the issue
            //           like calling APP_ERROR_CHECK to reset the device.
            APP_ERROR_CHECK(false);
            break;

        default:
        	NRF_LOG_INFO("Unknown event from ble_dfu_buttonless");
            break;
    }
}

static void advertising_init(void)
{
    ret_code_t    err_code;
    ble_advdata_t advdata;
    ble_advdata_t srdata;

    ble_uuid_t adv_uuids[] = {
    		// ToDo: причесать этот кусок циклом
		    {gtServices.tServices[0]->ptVars->tUuid.uuid, BLE_UUID_TYPE_VENDOR_BEGIN},
		    //{gtServices.tServices[1]->ptVars->tUuid.uuid, BLE_UUID_TYPE_VENDOR_BEGIN},
			{BLE_UUID_BATTERY_SERVICE, BLE_UUID_TYPE_BLE},
    };

    // Build and set advertising data
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type          = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance = true;
    advdata.flags              = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;

    memset(&srdata, 0, sizeof(srdata));
    srdata.uuids_complete.uuid_cnt = sizeof(adv_uuids) / sizeof(adv_uuids[0]);
    srdata.uuids_complete.p_uuids  = adv_uuids;

    err_code = ble_advdata_set(&advdata, &srdata);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for starting advertising.
 */
static void advertising_start(void)
{
    ret_code_t           err_code;
    ble_gap_adv_params_t adv_params;

    // Start advertising
    memset(&adv_params, 0, sizeof(adv_params));

    adv_params.type        = BLE_GAP_ADV_TYPE_ADV_IND;
    adv_params.p_peer_addr = NULL;
    adv_params.fp          = BLE_GAP_ADV_FP_ANY;
    adv_params.interval    = APP_ADV_INTERVAL;
    adv_params.timeout     = BLE_GAP_ADV_TIMEOUT_GENERAL_UNLIMITED;

    err_code = sd_ble_gap_adv_start(&adv_params, APP_BLE_CONN_CFG_TAG);
    APP_ERROR_CHECK(err_code);
}

static void gap_params_init(void)
{
    ret_code_t              err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    uint8_t* pubName = CustGetDeviceName();
    err_code = sd_ble_gap_device_name_set(&sec_mode, pubName, strlen(pubName));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MSEC_TO_UNITS(100, UNIT_1_25_MS); // 0.5s
    gap_conn_params.max_conn_interval = MSEC_TO_UNITS(200, UNIT_1_25_MS); // 1s
    gap_conn_params.slave_latency     = 0;
    gap_conn_params.conn_sup_timeout  = MSEC_TO_UNITS(4000, UNIT_10_MS); // 4s

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}

static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    ret_code_t err_code;

    NRF_LOG_DEBUG("Ble evt %x",p_ble_evt->header.evt_id);
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
        	NRF_LOG_INFO("Connected");
            // call on connect
        	RDevLedSetPattern(&BtConnLed);
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            break;

        case BLE_GAP_EVT_DISCONNECTED:
        	NRF_LOG_INFO("Disconnected");
        	RDevLedStopPattern(&BtConnLed);
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            advertising_start();
            break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            // Pairing not supported
            err_code = sd_ble_gap_sec_params_reply(m_conn_handle,
                                                   BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP,
                                                   NULL,
                                                   NULL);
            APP_ERROR_CHECK(err_code);
            break;

#ifndef S140
        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
        	NRF_LOG_INFO("PHY update request");
            ble_gap_phys_t const phys =
            {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
        } break;
#endif

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            // No system attributes have been stored.
            err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
        	NRF_LOG_INFO("GATT Client Timeout");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
        	NRF_LOG_INFO("GATT Server Timeout");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_EVT_USER_MEM_REQUEST:
            err_code = sd_ble_user_mem_reply(p_ble_evt->evt.gattc_evt.conn_handle, NULL);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
        {
            ble_gatts_evt_rw_authorize_request_t  req;
            ble_gatts_rw_authorize_reply_params_t auth_reply;

            req = p_ble_evt->evt.gatts_evt.params.authorize_request;

            if (req.type != BLE_GATTS_AUTHORIZE_TYPE_INVALID)
            {
                if ((req.request.write.op == BLE_GATTS_OP_PREP_WRITE_REQ)     ||
                    (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_NOW) ||
                    (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL))
                {
                    if (req.type == BLE_GATTS_AUTHORIZE_TYPE_WRITE)
                    {
                        auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
                    }
                    else
                    {
                        auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
                    }
                    auth_reply.params.write.gatt_status = APP_FEATURE_NOT_SUPPORTED;
                    err_code = sd_ble_gatts_rw_authorize_reply(p_ble_evt->evt.gatts_evt.conn_handle,
                                                               &auth_reply);
                    APP_ERROR_CHECK(err_code);
                }
            }
        } break; // BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST

        default:
            // No implementation needed.
            break;
    }
}

static void services_init(void)
{
    ret_code_t     err_code;
    ble_bas_init_t bas_init;
    ble_dis_init_t dis_init;

    // Here the sec level for the Battery Service can be changed/increased.
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&bas_init.battery_level_char_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&bas_init.battery_level_char_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&bas_init.battery_level_char_attr_md.write_perm);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&bas_init.battery_level_report_read_perm);

    bas_init.evt_handler          = NULL;
    bas_init.support_notification = true;
    bas_init.p_report_ref         = NULL;
    bas_init.initial_batt_level   = 100;

    err_code = ble_bas_init(&m_bas, &bas_init);
    APP_ERROR_CHECK(err_code);

    ble_dfu_buttonless_init_t dfus_init =
    {
        .evt_handler = ble_dfu_evt_handler
    };

    // Initialize the async SVCI interface to bootloader.
    err_code = ble_dfu_buttonless_async_svci_init();
    if(err_code == NRF_SUCCESS) {
    	err_code = ble_dfu_buttonless_init(&dfus_init);
    	APP_ERROR_CHECK(err_code);
    }

    uint8_t i = 0;
    while (gtServices.tServices[i]) {
    	err_code = CustomServiceInit(gtServices.tServices[i++]);
    	APP_ERROR_CHECK(err_code);
    }

    // Initialize Device Information Service
    memset(&dis_init, 0, sizeof(dis_init));
    nrf_dfu_settings_t* ptNrfDfuSettings = (nrf_dfu_settings_t*)BOOTLOADER_SETTINGS_ADDRESS;

    uint32_t ulVer = ptNrfDfuSettings->app_version;
    uint8_t v1 = ulVer/10000;
    uint8_t v2 = (ulVer-v1*10000)/100;
    uint8_t v3 = ulVer-v1*10000-v2*100;

    static uint8_t ubVerStr[12];
    dis_init.fw_rev_str.p_str = ubVerStr;
    dis_init.fw_rev_str.length = sprintf(ubVerStr, "%d.%d.%d", v1, v2, v3);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&dis_init.dis_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&dis_init.dis_attr_md.write_perm);

    err_code = ble_dis_init(&dis_init);
    APP_ERROR_CHECK(err_code);
}

static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
}

static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

void ble_stack_init()
{
    ret_code_t err_code;
    ble_conn_params_init_t cp_init;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);

    gap_params_init();

    err_code = nrf_ble_gatt_init(&m_gatt, NULL);
    APP_ERROR_CHECK(err_code);
    // ????
    //err_code = nrf_ble_gatt_att_mtu_periph_set(&m_gatt, 64);
    //APP_ERROR_CHECK(err_code);

    services_init();
    advertising_init();

    // Conn params
    memset(&cp_init, 0, sizeof(cp_init));
    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = APP_TIMER_TICKS(20000); // 15s
    cp_init.next_conn_params_update_delay  = APP_TIMER_TICKS(5000); // 5s
    cp_init.max_conn_params_update_count   = 3;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = true;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;
    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);

    advertising_start();

    battery_level_update(50);
}
