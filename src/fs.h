/*
 * fs.h
 *
 *  Created on: 15 груд. 2017 р.
 *      Author: ekrashtan
 */

#ifndef FS_H_
#define FS_H_


void FsInit(void);
ret_code_t FsWrite(uint32_t addr, uint8_t* data, uint32_t len);
ret_code_t FsRead(uint32_t addr, uint8_t* data, uint32_t len);
ret_code_t FsErase(uint32_t page, uint32_t len);
uint32_t nrf5_flash_end_addr_get(void);

#endif /* FS_H_ */
