
#include <stdbool.h>
#include <stdint.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf_drv_systick.h"
#include "bsp.h"


static void hw_init(void)
{
    bsp_board_leds_init();
    bsp_board_leds_off();
}

int main(void)
{
    hw_init();
    nrf_drv_systick_init();

    while (true)
    {
    	bsp_board_led_invert(0);
    	nrf_drv_systick_delay_ms(200);
    	bsp_board_led_invert(1);
    	nrf_drv_systick_delay_ms(500);
    	bsp_board_led_invert(2);
    	nrf_drv_systick_delay_ms(200);
    }
}

