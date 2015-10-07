#include <3ds.h>
#ifndef PAYLOAD_PATH
#include "payload_bin.h"
#endif
#include "brahma.h"


s32 main (void) {
	// Initialize services
	gfxInitDefault();
	gfxSwapBuffers(); 
	
	if (brahma_init()) {
		#ifdef PAYLOAD_PATH
		#ifdef PAYLOAD_OFFSET
		load_arm9_payload(PAYLOAD_PATH, PAYLOAD_OFFSET, 0);
		#else
		load_arm9_payload(PAYLOAD_PATH, 0, 0);
		#endif
		#else
		load_arm9_payload_from_mem(payload_bin, payload_bin_size);
		#endif
		firm_reboot();
		brahma_exit();
	}

    // deinitialize
	gfxExit();
    
	return 0;
}
