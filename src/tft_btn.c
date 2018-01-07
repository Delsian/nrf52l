/*
 * tft_btn.c
 *
 * TFT 14pin 1.44" ST7735S
 * pins:
 * 1 x
 * 2 3 4 5 NC
 * 6 SCK
 * 7 SDA
 * 8
 * 9
 * 10
 * 11
 *
 *==================
 * Buttons: Joystick 5 btns
 *
 * RGB LED
 *
 * pins:
 * 1
 * 2
 * 3
 * 4
 * 5 +RGB
 * 6
 * 7 Blue LED
 * 8
 * 9
 * 10
 * 11
 *
 *  Created on: Jan 7, 2018
 *      Author: Eug
 */


#include <stdint.h>
#include <stdbool.h>
#include "app_button.h"
#include "app_timer.h"
#include "boards.h"

static void JoystickEvtH(uint8_t iubPin, uint8_t iubAction);

const app_button_cfg_t tJoystick[] = {
	{ JOYSTICK_UP, APP_BUTTON_ACTIVE_LOW, NRF_GPIO_PIN_PULLUP, JoystickEvtH},
	{ JOYSTICK_DOWN, APP_BUTTON_ACTIVE_LOW, NRF_GPIO_PIN_PULLUP, JoystickEvtH},
	{ JOYSTICK_LEFT, APP_BUTTON_ACTIVE_LOW, NRF_GPIO_PIN_PULLUP, JoystickEvtH},
	{ JOYSTICK_CENTER, APP_BUTTON_ACTIVE_LOW, NRF_GPIO_PIN_PULLUP, JoystickEvtH},
};

#define BUTTON_DETECTION_DELAY          APP_TIMER_TICKS(50)

static void JoystickEvtH(uint8_t iubPin, uint8_t iubAction)
{
    ret_code_t err_code;

    switch (iubPin)
    {
        case JOYSTICK_UP:
            break;

        default:
            break;
    }
}

void InitTftBtn(void)
{
	ret_code_t tErr;

	tErr = app_button_init(tJoystick, sizeof(tJoystick) / sizeof(tJoystick[0]), BUTTON_DETECTION_DELAY);
	APP_ERROR_CHECK(tErr);
}
