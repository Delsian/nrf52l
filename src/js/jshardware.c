
#include <inttypes.h>
#include "jshardware.h"
#include "jsflash.h"

/**
 * Reads a byte from memory. Addr doesn't need to be word aligned and len doesn't need to be a multiple of 4.
 */
void jshFlashRead(void * buf, uint32_t addr, uint32_t len) {
	memcpy(buf, (void*)addr, len);
}

/// Return start address and size of the flash page the given address resides in. Returns false if no page.
bool jshFlashGetPage(uint32_t addr, uint32_t * startAddr, uint32_t * pageSize) {
	if (addr > (NRF_FICR->CODEPAGESIZE * NRF_FICR->CODESIZE))
		return false;
	*startAddr = (uint32_t) (floor(addr / NRF_FICR->CODEPAGESIZE) * NRF_FICR->CODEPAGESIZE);
	*pageSize = NRF_FICR->CODEPAGESIZE;
	return true;
}

// Just pass data through, since we can access flash at the same address we wrote it
size_t jshFlashGetMemMapAddress(size_t ptr) { return ptr; }

//JsfFileName jsfNameFromString(const char *name) {
//	assert(strlen(name)<=8);
//	char nameBuf[sizeof(JsfFileName)+1];
//	memset(nameBuf,0,sizeof(nameBuf));
//	strcpy(nameBuf,name);
//	return *(JsfFileName*)nameBuf;
//}

//JsVar *jsfReadFile(JsfFileName name) {
//	return 0;
//}
