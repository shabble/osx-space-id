/*
 * main.c
 * spacefinder
 *
 * + Created by shabble on 04/03/2010.
 * + Added animated transition, and refactored to
 *    use getopt, 20/6/2011.
 *
 */

#import <Carbon/Carbon.h>
#import <Foundation/Foundation.h>

#include <stdio.h>
#include <unistd.h>
#include "CGSPrivate.h"

/* definitions for a couple of useful functions, from
 * http://meeu.me/blog/dashboard-expose-spaces
 */
 
void CoreDockSendNotification(NSString *notificationName);
BOOL CoreDockGetWorkspacesEnabled();
void CoreDockGetWorkspacesCount(int *rows, int *columns);

void set_space(int space, BOOL animate) {
  
    if (animate) {
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        /* code for animated switching found here:
           http://meeu.me/blog/dashboard-expose-spaces
        */
        NSString *noteName = @"com.apple.switchSpaces";
        NSString *spaceID = [NSString stringWithFormat:@"%d", (int)space-1];
        NSNotificationCenter *dCenter = [NSDistributedNotificationCenter defaultCenter];
        [dCenter postNotificationName:noteName object:spaceID];
        
        [pool drain];
    } else {
        CGSConnection conn = _CGSDefaultConnection();
        CGSSetWorkspace(conn, space);
    }

}

int get_space() {
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
            return num;
        }
    }
}

void show_help_and_exit() {
    fprintf(stderr, "%s\n",
            "spacefinder: [-a -q -r -s <num>]\n"
            " -a animates space transition (only when -s is used)\n"
            " -q quiet mode, prints a single number with the current space ID\n"
            " -qq prints no output at all. Only useful with -r or -s\n"
            " -r encodes the space ID in the process return value\n"
            " -n returns the dimensions of the Spaces configuration\n"
            " -s <num> sets the current space to <num>. Reading args are not used\n"
            "  in conjunction with -s"

        );
    exit(0);
}

int main(int argc, char* argv[]) {
  
    BOOL quiet_mode         = NO;
    BOOL silent_mode        = NO;
    BOOL animate_transition = NO;
    BOOL use_return_value   = NO;
    BOOL should_set_space   = NO;
    BOOL display_dimensions = NO;

    int c;

    int rows = 0;
    int cols = 0;
    int num_spaces = 0;
    long space     = -1;

    if (!CoreDockGetWorkspacesEnabled()) {
        fprintf(stdout, "Spaces is not enabled\n");
        return -1;
    }

    CoreDockGetWorkspacesCount(&rows, &cols);
    num_spaces = rows * cols;

    while ((c = getopt(argc, argv, "hnrqas:")) != -1) {
        switch(c) {
        case 'h':
            show_help_and_exit();
            break;
            
        case 'n':
            display_dimensions = YES;
            break;
        case 'r':
            use_return_value = YES;
            break;
        case 'q':
            quiet_mode = YES;
            
            /* seen for the 2nd time */
            if (quiet_mode == YES) {
                silent_mode = YES;
            }
            break;
        case 'a':
            animate_transition = YES;
            break;
        case 's':
            if (strlen(optarg)) {
                should_set_space = YES;
                space = strtol(optarg, NULL, 10);
            } else {
                fprintf (stderr,
                         "Space number %s could not be parsed into a number",
                         optarg);
                return -1;
            }
            break;
        case '?':
            if (optopt == 's') {
                fprintf (stderr, "Option -%c requires an argument.\n", optopt);
            } else if (isprint (optopt)) {
                fprintf (stderr, "Unknown option `-%c'.\n", optopt);
            } else {
                fprintf (stderr,
                         "Unknown option character `\\x%x'.\n",
                         optopt);
            }
            return -1;
        }
    }
  
    if (space > 0 && should_set_space) { /* set space */
        if (space > num_spaces) {
            fprintf(stderr, "Cannot switch beyond maximum number of spaces\n");
            return -1;
        }

        set_space(space, animate_transition);

        if (!quiet_mode) {
            fprintf(stdout, "Switched to %d\n", space);
        }
        
        return 0;

    } else { /* get space */

        space = get_space();

        if (quiet_mode) {
            if (!silent_mode) {
                fprintf(stdout, "%d\n", space);
            }
        } else {
            fprintf(stdout, "Current Space ID: %d\n", space);
        }

        if (display_dimensions) {
            if (rows > 0 && cols > 0) {
                if (quiet_mode) {
                    if (!silent_mode) {
                        fprintf(stdout, "%dx%d\n", rows, cols);
                    }
                } else {
                    fprintf(stdout, "Spaces dimensions: %d rows, %d columns\n",
                            rows, cols);
                }
            } else {
                fprintf(stderr, "Could not determine the Spaces dimensions\n");
                return -1;
            }
        }

        if (use_return_value) {
            return space;
        } else {
            return 0;
        }
    }
}
