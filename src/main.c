
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "bluetooth.h"

#include "nrf_pwr_mgmt.h"
#include "app_timer.h"
#include "app_scheduler.h"
#include "nrf_soc.h"
#include "nrf_drv_clock.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#ifdef CLIRTT
#include "rtt_cli.h"
#endif

// Components
#include "boards.h"
#include "custom_service.h"
#include "pca9685.h"
#include "buzzer.h"

/**
 * Keep powered after pressing button 'On'
 */
static void power_on()
{
	nrf_gpio_cfg_output(PWR_ON);
	nrf_gpio_pin_set(PWR_ON);
}

static void hw_init(void)
{
	ret_code_t err_code;

#ifdef CLIRTT
	rtt_cli_init();
#endif

    /* initializing the Power manager. */
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_clock_lfclk_request(NULL);

    // Initialize timer module, making it use the scheduler
    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    power_on();
}

int main(void)
{
    hw_init();
    APP_SCHED_INIT(sizeof(void*), 16);

    //PcaInit();
    BuzzerInit();

    ble_stack_init();

    BuzzerPlayTone(70);
    nrf_delay_ms(500);
    BuzzerPlayTone(0);

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
