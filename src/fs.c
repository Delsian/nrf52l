/*
 * fs.c
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "nrf.h"
#include "nrf_log.h"
#include "nrf_soc.h"
#include "app_error.h"
#include "nordic_common.h"
#include "nrf_sdh.h"
#include "nrf_fstorage.h"
#include "nrf_fstorage_sd.h"

static void fstorage_evt_handler(nrf_fstorage_evt_t * p_evt);
static nrf_fstorage_api_t * p_fs_api = &nrf_fstorage_sd;
static uint8_t* pubWrPtr;

NRF_FSTORAGE_DEF(nrf_fstorage_t fstorage) =
{
    /* Set a handler for fstorage events. */
    .evt_handler = fstorage_evt_handler,

    /* These below are the boundaries of the flash space assigned to this instance of fstorage.
     * You must set these manually, even at runtime, before nrf_fstorage_init() is called.
     * The function nrf5_flash_end_addr_get() can be used to retrieve the last address on the
     * last page of flash available to write data. */
    .start_addr = 0x3e000,
    .end_addr   = 0x80000,
};

uint32_t nrf5_flash_end_addr_get()
{
    uint32_t const bootloader_addr = NRF_UICR->NRFFW[0];
    uint32_t const page_sz         = NRF_FICR->CODEPAGESIZE;
    uint32_t const code_sz         = NRF_FICR->CODESIZE;

    return (bootloader_addr != 0xFFFFFFFF ?
            bootloader_addr : (code_sz * page_sz));
}

static void fstorage_evt_handler(nrf_fstorage_evt_t * p_evt)
{
    if (p_evt->result != NRF_SUCCESS)
    {
    	NRF_LOG_ERROR("FStorage %x", p_evt->result);
    }

    switch (p_evt->id)
    {
        case NRF_FSTORAGE_EVT_WRITE_RESULT:
        	if (p_evt->p_param) {
        		free(p_evt->p_param);
        	}
        	break;
        case NRF_FSTORAGE_EVT_ERASE_RESULT:
        	break;
        default:
            break;
    }
}

// to write static data
// data buffer will have extra space to fit full words
ret_code_t FsWrite(uint32_t addr, uint8_t* data, uint32_t len)
{
	uint32_t nlen = (len+3)&0xFFFFFFFC;
	void* buf = malloc(nlen);
	memset(buf+(nlen-4), 0xFF, 3); // Fill last bytes to word size
	memcpy(buf, data, len);
	NRF_LOG_DEBUG("Wr %d(%p) at %x", nlen, buf, addr);
	ret_code_t err = nrf_fstorage_write(&fstorage, addr, buf, nlen, buf);
	if (err) NRF_LOG_ERROR("FsWrite Err %x", err);
	return err;
}

// If we have dynamically allocated buffer
ret_code_t FsWriteFree(uint32_t addr, uint8_t* data, uint32_t len)
{
	while (nrf_fstorage_is_busy(&fstorage));
	ret_code_t err = nrf_fstorage_write(&fstorage, addr, data, len, data);
	if (err) NRF_LOG_ERROR("FsWriteFree Err %x", err);
	return err;
}

ret_code_t FsRead(uint32_t addr, uint8_t* data, uint32_t len)
{
	return nrf_fstorage_read(&fstorage, addr, data, len);
}

ret_code_t FsErase(uint32_t page, uint32_t len)
{
	ret_code_t err = nrf_fstorage_erase(&fstorage, page, len, NULL);
	if (err) NRF_LOG_ERROR("FsErase Err %x", err);
	return err;
}

void FsInit()
{
    ret_code_t rc;
    rc = nrf_fstorage_init(&fstorage, p_fs_api, NULL);
    APP_ERROR_CHECK(rc);

}
