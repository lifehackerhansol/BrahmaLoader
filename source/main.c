#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#ifndef PAYLOAD_PATH
#include "payload_bin.h"
#endif
#include "hid.h"
#include "brahma.h"


void error_show(const char *format, ...) {
    consoleInit(GFX_BOTTOM, NULL);
    
    printf("* BRAHMA2LOADER *\n\n");
    va_list va;
    va_start(va, format);
    vprintf(format, va);
    va_end(va);
    
    wait_any_key();
}

s32 voodoo_load(const char* path, u32 offset, u32 msize, u32 voodoo) {
    u32 res = 0;
    u32 boot_delay = ((voodoo>>2)&0x3) * 50;
    u32 magic_fix = voodoo & (1<<1);
    u32 load_svc = voodoo & (1<<0);
    
    // load services
    if (load_svc) {
        srvInit();
        aptInit();
        hidInit();
        acInit();
        ptmInit();
    }
    
    // magic fix
    if (magic_fix) {
        // offset potential issues caused by homebrew that just ran
        aptOpenSession();
        APT_SetAppCpuTimeLimit(NULL, 0);
        aptCloseSession();
    }
    
    // bootfix delay
    if (boot_delay) {
        while (aptMainLoop() && boot_delay-- > 0) {
            gfxFlushBuffers();
            gfxSwapBuffers();
            gspWaitForVBlank();
        }
    }
    
    // actually load the payload, for once
    if (path && path[0] != '\0') {
        res = load_arm9_payload(path, offset, msize);
        if (!res) error_show("[+] payload: %s@%X\n[+] maximum size: %X\n[!] Loading failed!", path, offset, msize);
    }
    #ifndef PAYLOAD_PATH
    else {
        res = load_arm9_payload_from_mem(payload_bin, payload_bin_size);
        if (!res) error_show("[+] payload from mem\n[!] Loading failed!");
    }
    #endif
    
    // unload services
    if (load_svc) {
        ptmExit();
        acExit();
        hidExit();
        aptExit();
        srvExit();
    }
    
    return res;
}

s32 main (int argc, char **argv) {
    char path[128] = { 0 };
    u32 offset = 0;
    u32 msize = 0;
    u32 voodoo = 0;
       
	// Initialize GFX services
	gfxInitDefault();
	gfxSwapBuffers();

	if (brahma_init()) {
		// Get payload parameters
        #ifdef VOODOO
        voodoo = VOODOO;
        #endif
        #ifdef PAYLOAD_PATH
        strncpy(path, PAYLOAD_PATH, 128);
        #ifdef PAYLOAD_OFFSET
        offset = PAYLOAD_OFFSET;
        #endif
        #ifdef PAYLOAD_MAXSIZE
        msize = PAYLOAD_MAXSIZE;
        #endif
        #else
        if (argc > 1) {
            s32 res = sscanf(argv[1], "%128s %X %X %X", path, &offset, &msize, &voodoo);
            if (res < 4) voodoo = 0;
            if (res < 3) msize = 0;
            if (res < 2) offset = 0;
            if (res < 1) error_show("[!] Invalid argument: %s", argv[1]);
            else if (voodoo_load(path, offset, msize, voodoo)) firm_reboot();
        } else if (voodoo_load(path, offset, msize, voodoo))
            firm_reboot();
        #endif
		brahma_exit();
	} else error_show("[!] Not enough memory");

    // Deinitialize GFX
	gfxExit();
    
	return 0;
}
