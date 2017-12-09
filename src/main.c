
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "bluetooth.h"

#include "nrf_pwr_mgmt.h"
#include "app_timer.h"
#include "boards.h"
#include "bsp.h"

void swo_init(void);

static void hw_init(void)
{
	ret_code_t err_code;

	swo_init(); // logging

    bsp_board_leds_init();
    bsp_board_leds_off();

    /* initializing the Power manager. */
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
    // Initialize timer module, making it use the scheduler
    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}

int main(void)
{
	ret_code_t err_code;

    hw_init();
    printf("Blinky");
    if ((err_code = ble_init()) == NRF_SUCCESS) {
    	printf("Blinky example started.");

		while (1) {

		}
    } else {
    	printf("Ble error 0x%x", err_code);
    }
}

void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(0xDEADBEEF, line_num, p_file_name);
}
