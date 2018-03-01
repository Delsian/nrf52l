
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
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

// Components
#include "fs.h"
#include "boards.h"
#include "control.h"
#include "custom_service.h"
#include "buzzer.h"
#include "r0b1c_device.h"

static void HwInit(void)
{
	ret_code_t err_code;

    /* initializing the Power manager. */
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_clock_lfclk_request(NULL);

    // Initialize timer module, making it use the scheduler
    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}

RDevErrCode RDevLedTick(uint8_t port, uint32_t time);

int main(void)
{
    (void) NRF_LOG_INIT(NULL);
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    HwInit();

    ControlInit();
    FsInit();

    RDeviceInit();
//    while(1) {
//    	RDevLedTick(0,0);
//    }

    BuzzerInit();

    ble_stack_init();

    BuzzerPlayTone(70);
    nrf_delay_ms(100);
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
