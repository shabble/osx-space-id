/*
 * main.c
 * spacefinder
 *
 * + Created by shabble on 04/03/2010.
 * + Added animated transition, thanks to Denis Gryzlov,
 *    and refactored to use getopt, 20/6/2011.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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

void set_space(long space, BOOL animate) {

    if (animate) {
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        /* code for animated switching found here:
           http://meeu.me/blog/dashboard-expose-spaces
        */
        NSString *noteName = @"com.apple.switchSpaces";
        NSString *spaceID = [NSString stringWithFormat:@"%li", space - 1];
        NSNotificationCenter *dCenter = [NSDistributedNotificationCenter defaultCenter];
        [dCenter postNotificationName:noteName object:spaceID];

        [pool drain];
    } else {
        CGSConnection conn = _CGSDefaultConnection();
        CGSSetWorkspace(conn, space);
    }

}

int get_space_via_keywin() {
    CFArrayRef winList =
        CGWindowListCopyWindowInfo(kCGWindowListOptionOnScreenOnly, kCGNullWindowID);

    int len = CFArrayGetCount(winList);
    int i, num = 0;
    for (i = 0; i < len; i++) {
        CFDictionaryRef winDict = CFArrayGetValueAtIndex(winList, i);
        if (CFDictionaryContainsKey(winDict, kCGWindowWorkspace)) {
            const void *thing = CFDictionaryGetValue(winDict, kCGWindowWorkspace);
            CFNumberRef numRef = (CFNumberRef)thing;
            CFNumberGetValue(numRef, kCFNumberIntType, &num);
            return num;
        }
    }
    return -1;
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
            /* seen for the 2nd time */
            if (quiet_mode == YES) {
                silent_mode = YES;
            }
            quiet_mode = YES;
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
            if (!isprint(optopt)) {
                fprintf(stderr,
                        "Unknown option character `\\x%x'.\n",
                        optopt);
            }
            return -1;
        }
    }

/*    printf("Args: dims:%d set:%d quiet:%d silent:%d ret:%d\n",
           display_dimensions, should_set_space, quiet_mode,
           silent_mode, use_return_value); */

    if (space > 0 && should_set_space) { /* set space */
        if (space > num_spaces) {
            fprintf(stderr, "Cannot switch beyond maximum number of spaces\n");
            return -1;
        }

        set_space(space, animate_transition);

        if (!quiet_mode) {
            fprintf(stdout, "Switched to %li\n", space);
        }

        return 0;

    } else { /* get space */

        space = get_space_via_keywin();

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
        } else {
            if (quiet_mode) {
                if (!silent_mode) {
                    fprintf(stdout, "%li\n", space);
                }
            } else {
                fprintf(stdout, "Current Space ID: %li\n", space);
            }
        }

        if (use_return_value) {
            return space;
        } else {
            return 0;
        }
    }
}
