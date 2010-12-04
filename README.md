OSX Spaces Current Workspace
============================

[Original code is from shabble, with some updates from canadaduane]

For a variety of useful and useless purposes when scripting in OSX, it
might be handy to know which of the  Spaces (virtual desktops)
you're currently on. You may also wish to programmatically switch spaces
from the command line.

To the best of my knowledge, the necessary ID is not programatically accessible
without jumping through a few hoops.

[Meeu Labs](http://meeu.me/blog/dashboard-expose-spaces) has some interesting
information regarding hacking undocumented Mac OS X APIs.  This command-line
tool currently uses [CGSPrivate.h](http://code.google.com/p/undocumented-goodness/source/browse/trunk/CoreGraphics/CGSPrivate.h)
to switch spaces.  It suffers from two signficant drawbacks, however:
  1. there is no transition animation
  2. focus remains on the window before the switch

[Mac OSX Hints](http://www.macosxhints.com/article.php?story=20080227075244778)
has one possible solution, which uses the Assistive Devices support to rip the
value from the menubar icon, but this is really horribly hacky, and pretty slow,
at least for me. It takes at least a second, and more like 2 or 3 typically.

An alternative idea is presented on
[StackOverflow](http://stackoverflow.com/questions/554380/how-to-detect-which-space-the-user-is-on-in-mac-os-x-leopard)
which uses some of the (probably horribly deprecated) Carbon APIs fetch a window
handle, and then poke around in its internal dictionary to find the workspace
ID.

The latter is the method implemented here.

Usage
-----

Compile with:

`$ make`

and run with:

`$ ./spacefinder`

The program should terminate immediately, printing a string to `STDOUT`
which matches the regex `/^Current Space ID: \d+$/`. The current space
number is also the return code of the program for ease of use in scripting
apps.  If the space cannot be identified for whatever reason, it will be
returned as `0`.

If you call spacefinder with the `-q` option, it will enter quiet mode and
only print the space number (matching `/^\d+$/`)

I have no idea what happens if Spaces are disabled, or if you manage to invoke
it from the Spaces Expose screen, or other weird edge cases. Submissions
welcomed.

I have no idea if this will behave correctly on Snow Leopard, I'm still running
10.5.8
