#include <3ds.h>
#include "payload_bin.h"
#include "brahma.h"


s32 main (void) {
	// Initialize services
	gfxInitDefault();
	gfxSwapBuffers(); 
	
	if (brahma_init()) {
		// load_arm9_payload("payload.bin");
		load_arm9_payload_from_mem(payload_bin, payload_bin_size);
		firm_reboot();
		brahma_exit();
	}

	gfxExit();
	// Return to hbmenu
	return 0;
}
