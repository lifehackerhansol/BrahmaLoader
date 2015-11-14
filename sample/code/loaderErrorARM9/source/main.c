#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "draw.h"
#include "hid.h"


void ClearTop() {
	ClearScreen(TOP_SCREEN0, RGB(255, 255, 255));
	ClearScreen(TOP_SCREEN1, RGB(255, 255, 255));
	current_y = 0;
}

void wait_key() {
	Debug("");
	Debug("Press key to continue");
	InputWait();
}

int main() {
	ClearTop();
    Debug("");
	Debug("Seems like something went wrong here!");
    Debug("");
    Debug("Next time, you should specify a payload.");
    Debug("To do so you can... (before compiling):");
    Debug("- put a payload.bin file into data/.");
    Debug("- edit the AppInfo file in resources/.");
    Debug("");
    Debug("You may also use *hax 2.5 with XML start");
    Debug("parameters to specify an ARM9 payload.");
	wait_key();

	// return control to FIRM ARM9 code (performs firmlaunch)
	return 0;
}