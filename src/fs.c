/*
 * fs.c
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "nrf.h"
#include "nrf_soc.h"
#include "nordic_common.h"
#include "nrf_sdh.h"
#include "nrf_fstorage.h"
#include "nrf_fstorage_sd.h"

static void fstorage_evt_handler(nrf_fstorage_evt_t * p_evt);
static nrf_fstorage_api_t * p_fs_api = &nrf_fstorage_sd;

NRF_FSTORAGE_DEF(nrf_fstorage_t fstorage) =
{
    /* Set a handler for fstorage events. */
    .evt_handler = fstorage_evt_handler,

    /* These below are the boundaries of the flash space assigned to this instance of fstorage.
     * You must set these manually, even at runtime, before nrf_fstorage_init() is called.
     * The function nrf5_flash_end_addr_get() can be used to retrieve the last address on the
     * last page of flash available to write data. */
    .start_addr = 0x3e000,
    .end_addr   = 0x3ffff,
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
        printf("--> Event received: ERROR while executing an fstorage operation.\n");
        return;
    }

    switch (p_evt->id)
    {
        case NRF_FSTORAGE_EVT_WRITE_RESULT:
        	break;
        case NRF_FSTORAGE_EVT_ERASE_RESULT:
        	break;
        default:
            break;
    }
}

static void wait_for_flash_ready(nrf_fstorage_t const * p_fstorage)
{
    while (nrf_fstorage_is_busy(p_fstorage))
    {
		vTaskDelay(1);
    }
}

ret_code_t fs_write(uint16_t addr, uint8_t* data, uint32_t len)
{
	wait_for_flash_ready(&fstorage);
	return nrf_fstorage_write(&fstorage, addr, data, len, NULL);
}

ret_code_t fs_read(uint16_t addr, uint8_t* data, uint32_t len)
{
	wait_for_flash_ready(&fstorage);
	return nrf_fstorage_read(&fstorage, addr, data, len);
}

ret_code_t fs_erase(uint16_t page, uint32_t len)
{
	wait_for_flash_ready(&fstorage);
	return nrf_fstorage_erase(&fstorage, page, len, NULL);
}

void fs_init()
{
    ret_code_t rc;

    rc = nrf_fstorage_init(&fstorage, p_fs_api, NULL);
    APP_ERROR_CHECK(rc);

}
