
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "bluetooth.h"

#include "nrf_pwr_mgmt.h"
#include "app_timer.h"
#include "app_scheduler.h"
#include "nrf_soc.h"
#include "nrf_clock.h"
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
#include "rdev_proto.h"
#include "js_module.h"


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
			// Shut down request
			BtnEvt.type = CE_PWR_OFF_REQ;
			ControlPost(&BtnEvt);
			usBtnPressed = BUTTON_SHORT_PRESS; // Re-arm for next request (if not powered off)
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

static void clock_irq_handler(nrfx_clock_evt_type_t evt)
{
#if 0
    if (evt == NRFX_CLOCK_EVT_HFCLK_STARTED)
    {
        m_clock_cb.hfclk_on = true;
        clock_clk_started_notify(NRF_DRV_CLOCK_EVT_HFCLK_STARTED);
    }
    if (evt == NRFX_CLOCK_EVT_LFCLK_STARTED)
    {
        m_clock_cb.lfclk_on = true;
        clock_clk_started_notify(NRF_DRV_CLOCK_EVT_LFCLK_STARTED);
    }
#if CALIBRATION_SUPPORT
    if (evt == NRFX_CLOCK_EVT_CTTO)
    {
        nrf_drv_clock_hfclk_request(&m_clock_cb.cal_hfclk_started_handler_item);
    }

    if (evt == NRFX_CLOCK_EVT_CAL_DONE)
    {
        nrf_drv_clock_hfclk_release();
        bool aborted = (m_clock_cb.cal_state == CAL_STATE_ABORT);
        m_clock_cb.cal_state = CAL_STATE_IDLE;
        if (m_clock_cb.cal_done_handler)
        {
            m_clock_cb.cal_done_handler(aborted ?
                NRF_DRV_CLOCK_EVT_CAL_ABORTED : NRF_DRV_CLOCK_EVT_CAL_DONE);
        }
    }
#endif // CALIBRATION_SUPPORT
#endif
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

    err_code = nrfx_clock_init(clock_irq_handler);
    APP_ERROR_CHECK(err_code);

    //nrf_drv_clock_lfclk_request(NULL);

    // Initialize timer module, making it use the scheduler
    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    ButtonInit();
}

int main(void)
{
    (void) NRF_LOG_INIT(NULL);
    NRF_LOG_DEFAULT_BACKENDS_INIT();
    NRF_LOG_INFO("main()");

    HwInit();

    ControlInit();
    FsInit();

    //RDeviceInit();

    BuzzerInit();

    ble_stack_init();

    JsInit();

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
