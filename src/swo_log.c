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
    ITM_SendChar((uint8_t)ch);
    return ch;
}

int _write(int file, const char * p_char, int len)
{
    int i;
    for (i = 0; i < len; i++)
    	ITM_SendChar(*p_char++);
    return len;
}

int _write_r(struct _reent *r, int file, char *ptr, int len) {
    for (int i = 0; i < len; i++)
        ITM_SendChar(ptr[i]);
    return len;
}

int fputc(int ch, FILE * p_file)
{
	ITM_SendChar((uint8_t)ch);
    return ch;
}
