#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "brahma.h"
#include "hid.h"
#include "menus.h"
#include "sochlp.h"

void interact_with_user (void) {
	s32 menuidx = 0;
	
	while (aptMainLoop()) {
		gspWaitForVBlank();

		menuidx = print_main_menu(menuidx, &g_main_menu);		

		u32 kDown = wait_key(); 
		
		if (kDown & KEY_B) {
			break;
		}
		else if (kDown & KEY_A) {
			consoleClear();
			printf("\n");
			if (menu_execute_function(menuidx, &g_main_menu, 0))
				wait_any_key();
		}
		else if (kDown & KEY_UP) {
			menuidx--;
		}
		else if (kDown & KEY_DOWN) {
			menuidx++;
		}
		gfxFlushBuffers();
		gfxSwapBuffers(); 
	}

	return;
}

s32 quick_boot_firm (s32 load_from_disk) {
	if (load_from_disk)
		load_arm9_payload("/arm9payload.bin", 0, 0);
	firm_reboot();
}

s32 main (int argc, char **argv) {
	// Initialize services
	gfxInitDefault();
	gfxSwapBuffers(); 
	
	consoleInit(GFX_BOTTOM, NULL);
	if (brahma_init()) {
		if (argc > 1) {
			char payload[128];
			u32 offset;
			u32 psize;
			
			u32 res = sscanf(argv[1], "%128s %X %X", payload, &offset, &psize);
			if (res < 3) psize = 0;
			if (res < 2) offset = 0;
			if (res < 1) {
				printf("[!] Invalid argument: %s\n", argv[1]);
			} else {
				printf("[+] payload: %s@%X\n[+] maximum size: %X\n", payload, offset, psize);
				if (load_arm9_payload(payload, offset, psize))
					firm_reboot();
				printf("[!] Loading failed\n");
			}
			wait_any_key();
		} else {
			hidScanInput();
			u32 kHeld = hidKeysHeld();
		 
			if (kHeld & KEY_LEFT) {
				/* load default payload from dosk and run exploit */
				quick_boot_firm(1);
				printf("[!] Quickload failed\n");
				wait_any_key();
			} else if (kHeld & KEY_RIGHT) {
				/* reboot only */
				quick_boot_firm(0);
			}

			soc_init();	
			interact_with_user();
			soc_exit();
			brahma_exit();
		}
	} else {
		printf("* BRAHMA *\n\n[!]Not enough memory\n");
		wait_any_key();
	}

	gfxExit();
	// Return to hbmenu
	return 0;
}
