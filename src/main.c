
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


#define BUTTON_SHORT_PRESS 4
#define BUTTON_LONG_PRESS 400
#define BUTTON_TICK_MS 5
APP_TIMER_DEF(tBtnTimer);

static void ButtonTickHandler()
{
	static uint16_t usBtnPressed;
	static ControlEvent BtnEvt;

	if (nrf_gpio_pin_read(BUTTON1) == 0) { // if pressed
		if (++usBtnPressed == BUTTON_SHORT_PRESS) {
			// send notif
			BtnEvt.type = CE_BUTTON;
			BtnEvt.b = true;
			ControlPost(&BtnEvt);
		}
		if (usBtnPressed > BUTTON_LONG_PRESS) {
			// Shut down, no more checking
			app_timer_stop(tBtnTimer);
			BtnEvt.type = CE_PWR_OFF;
			ControlPost(&BtnEvt);
		}
	} else {
		if (usBtnPressed > BUTTON_SHORT_PRESS) {
			// send notif
			BtnEvt.type = CE_BUTTON;
			BtnEvt.b = false;
			ControlPost(&BtnEvt);
		}
		usBtnPressed = 0;
	}
}

static void ButtonInit()
{
	nrf_gpio_cfg_input(BUTTON1, NRF_GPIO_PIN_PULLUP);
	app_timer_create(&tBtnTimer, APP_TIMER_MODE_REPEATED, ButtonTickHandler);
	app_timer_start(tBtnTimer, APP_TIMER_TICKS(BUTTON_TICK_MS), NULL);
}

static void HwInit(void)
{
	ret_code_t err_code;

	/* Enable power switch */
	nrf_gpio_cfg_output(PWR_ON);
	nrf_gpio_pin_set(PWR_ON);

    /* initializing the Power manager. */
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_clock_lfclk_request(NULL);

    // Initialize timer module, making it use the scheduler
    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    ButtonInit();
}

void js_run(void);
int main(void)
{
    (void) NRF_LOG_INIT(NULL);
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    HwInit();

    ControlInit();
    FsInit();

    js_run();
    while (1);

    RDeviceInit();

    BuzzerInit();

    ble_stack_init();

    BuzzerPlayTone(500);
    nrf_delay_ms(50);
    BuzzerPlayTone(400);
    nrf_delay_ms(70);
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
