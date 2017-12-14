
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "bluetooth.h"
#include "control.h"
#include "leds.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include "nrf_pwr_mgmt.h"
#include "app_timer.h"
#include "boards.h"
#include "nrf_soc.h"
#include "rtt_cli.h"

#ifdef USE_LUA
#include "nlua.h"
#endif

static SemaphoreHandle_t m_ble_event_ready;  /**< Semaphore raised if there is a new event to be processed in the BLE thread. */
static TaskHandle_t  m_ble_stack_thread;     /**< Definition of BLE stack thread. */

static void hw_init(void)
{
	ret_code_t err_code;

#ifdef CLIRTT
	rtt_cli_init();
#endif

    /* initializing the Power manager. */
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
    // Initialize timer module, making it use the scheduler
    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}

int main(void)
{
    hw_init();
    printf("Blinky\n");
    control_init();
    leds_init();

    m_ble_event_ready = xSemaphoreCreateBinary();
    if(NULL == m_ble_event_ready)
    {
        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
    }

#ifdef USE_LUA
    nlua_init();
#endif

    if(pdPASS != xTaskCreate(ble_stack_thread, "BLE", 256, NULL, 1, &m_ble_stack_thread))
    {
        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
    }

    // Start FreeRTOS scheduler.
    vTaskStartScheduler();
}

void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(0xDEADBEEF, line_num, p_file_name);
}
