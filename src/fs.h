/*
 * fs.h
 *
 *  Created on: 15 груд. 2017 р.
 *      Author: ekrashtan
 */

#ifndef FS_H_
#define FS_H_


void fs_init(void);
ret_code_t fs_write(uint16_t addr, uint8_t* data, uint32_t len);
ret_code_t fs_read(uint16_t addr, uint8_t* data, uint32_t len);
ret_code_t fs_erase(uint16_t page, uint32_t len);
uint32_t nrf5_flash_end_addr_get(void);

#endif /* FS_H_ */
