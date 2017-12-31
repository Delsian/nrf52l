
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "bluetooth.h"
#include "control.h"
#include "leds.h"
#include "esp_impl.h"

#include "nrf_pwr_mgmt.h"
#include "app_timer.h"
#include "app_scheduler.h"
#include "boards.h"
#include "nrf_soc.h"
#include "rtt_cli.h"
#include "custom_service.h"

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
    APP_SCHED_INIT(sizeof(void*), 6);
    leds_init();

    ble_stack_init();

	while (1)
	{
		app_sched_execute();
		(void)sd_app_evt_wait();
	}
}

void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(0xDEADBEEF, line_num, p_file_name);
}
