//
//  main.c
//  spacefinder
//
//  Created by shabble on 04/03/2010.
//  
#include <Carbon/Carbon.h>
#include <stdio.h>

int main(int argc, char* argv[]) {

    int quiet_mode = 0;

    /* process the 'quiet' option */
    if (argc > 1) {
        if (strncmp(argv[1], "-q", 2) == 0) {
            quiet_mode = 1;
        }
    }

 	CFArrayRef winList = 
	    CGWindowListCopyWindowInfo(kCGWindowListOptionOnScreenOnly, kCGNullWindowID);
	int len = CFArrayGetCount(winList);
	int i, num = 0;
	for (i = 0; i < len; i++) {
		CFDictionaryRef winDict = CFArrayGetValueAtIndex(winList, i);
		if (CFDictionaryContainsKey(winDict, kCGWindowWorkspace)) {
			//fprintf(stderr, "Yay, contains that key for i: %d\n", i);
			const void *thing = CFDictionaryGetValue(winDict, kCGWindowWorkspace);
			CFNumberRef numRef = (CFNumberRef)thing;
			CFNumberGetValue(numRef, kCFNumberIntType, &num);
  
            if (quiet_mode == 1) {
                fprintf(stdout, "%d\n", num);
            } else {
                fprintf(stdout, "Current Space ID: %d\n", num);
            }

			break;
		}
	}

	return num;
}
