/*
 * r0b1c_cmd.h
 *
 *  Created on: 15.02.18
 *      Author: ekrashtan
 */

#ifndef R0B1C_CMD_H_
#define R0B1C_CMD_H_
#include "ble.h"
#include "r0b1c_device.h"

void OnPortWriteEvt(ble_evt_t const * p_ble_evt);
void OnCmdWriteEvt(ble_evt_t const * p_ble_evt);
void CmdInitComplete(void);
void CmdErrWrite(RDevErrCode itErrCode);
void SendCmdResp(uint8_t* ipubData, uint8_t iubLen);

#endif /* R0B1C_CMD_H_ */
