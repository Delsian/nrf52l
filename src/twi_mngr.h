/*
 * twi_mngr.h
 *
 *  Created on: 2 бер. 2018 р.
 *      Author: ekrashtan
 */

#ifndef TWI_MNGR_H_
#define TWI_MNGR_H_
#include "nrf_drv_twi.h"

void TwiMngrInit(void);
nrf_drv_twi_t const * TwiGetDrv(void);


#endif /* TWI_MNGR_H_ */
