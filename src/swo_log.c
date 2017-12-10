/*
 * swo_log.c
 *
 *  Created on: Dec 8, 2017
 *      Author: Eug
 */

#include <stdint.h>
#include <stdio.h>
#include "nrf.h"

void swo_init()
{
	NRF_CLOCK->TRACECONFIG = (NRF_CLOCK->TRACECONFIG & ~CLOCK_TRACECONFIG_TRACEPORTSPEED_Msk) |
	        (CLOCK_TRACECONFIG_TRACEPORTSPEED_4MHz << CLOCK_TRACECONFIG_TRACEPORTSPEED_Pos);
	ITM->TCR |= 1;
	ITM->TER |= 1;
}

int __putchar(int ch, FILE * p_file)
{
    ITM_SendChar(ch);
    return ch;
}

int _write(int file, const char * p_char, int len)
{
    int i;

    for (i = 0; i < len; i++)
    {
    	ITM_SendChar(*p_char++);
    }

    return len;
}
