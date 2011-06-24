# Command-line OSX Spaces Manipulation Tool

Original code is by [shabble](https://github.com/shabble), with some updates
from [canadaduane](https://github.com/chicagoduane), animated switching code
and dimension retrieval inspired by a blog post from
[Denis Gryzlov](http://meeu.me/blog/dashboard-expose-spaces).

For a variety of useful and useless purposes when scripting in OSX, it
might be handy to know which of the  Spaces (virtual desktops)
you're currently on. You may also wish to programmatically switch spaces
from the command line.

[Mac OSX Hints](http://www.macosxhints.com/article.php?story=20080227075244778)
has one possible solution, which uses the Assistive Devices support to rip the
value from the menubar icon, but this is really horribly hacky, and pretty slow,
at least for me. It takes at least a second, and more like 2 or 3 typically.

An alternative idea is presented on
[StackOverflow](http://stackoverflow.com/questions/554380/how-to-detect-which-space-the-user-is-on-in-mac-os-x-leopard)
which uses some Carbon API calls to fetch a window handle of the currently
focused app (which is presumably on your current space), and then poke around in
its internal metadata dictionary to find its workspace ID.

The latter is the method implemented here.

## Compilation

Compile with:

`$ make`

and run with:

`$ ./spacefinder`

or copy it to somewhere in your `$PATH`.  There is currently no install target
in the Makefile.

## Usage & Options

* `-a` animates space transition (only when `-s` is used)
* `-q` quiet mode, prints a single number with the current space ID
* `-qq` prints no output at all. Only useful with `-r` or `-s`
* `-r` encodes the space ID in the process return value
* `-n` returns the dimensions of the Spaces configuration
* `-s <num>` sets the current space to `<num>`. Any 'fetch' configuring
  arguments are ignored when used in conjunction with `-s`

### Fetching Spaces ID

The program should terminate immediately, printing a string to `STDOUT`
which matches the regex `/^Current Space ID: \d+$/`. If you pass the
`-r` option, the return value of the program is set to the current Space number
for ease of use in scripting.

For example, in Bash: `$ ./spacefinder -r -qq; echo $?` will print the Space
number apps.  If the space cannot be determined for any reason, it will be
returned as -1 (255).

If you call spacefinder with the `-q` option, it will enter quiet mode and
only print the space number (matching `/^\d+$/`). The silent version (`-qq`)
will print nothing to `STDOUT`, and any error messages will be printed to
`STDERR`.  Silent is generally only useful if you're switching Spaces, or
planning to use the return value.

### Fetching Spaces Dimensions

The `-n` flag returns one of two strings, depending on whether quiet mode
is enabled.  If quiet is enabled, it will print `/^\dx\d$/`, where each
number is a single digit in the range 1 .. 4.

If quiet is not enabled, the output is more verbose:
`/^Spaces dimensions: \d rows, \d columns$/`

In silent mode, nothing is printed at all.

The return value is the current Space number (if using `-r`), or 0 for success.
It is set to -1 (255) for any failure.

### Setting Spaces ID

spacefinder now supports switching to other Spaces via the `-s` option.
Specifying `-s <num>` on its own will switch directly to that space, without
any transitional animation. The `-a` (animate) flag enables the usual "slide"
transition effect.

Quiet and silent mode can be used with with `-s` option, otherwise, it will
print a message `/^Switched to \d+/` to `STDOUT`, or an error message to
`STDERR` if it fails.

The return value is **NOT** set to the Space number when using the set feature, 
it is either 0 (for successful operation), or -1 (generally represented as 255)
on error.

Attempting to set the Space number beyond the number of spaces available will
result in an error. Spaces are numbered from 1 (top-left), to _N_, where _N_ is
_rows_ * _columns_ of your Spaces setup.

## Caveats

I have no idea what happens if Spaces are disabled, or if you manage to invoke
it from the Spaces Expose screen, or other weird edge cases. Submissions
welcomed.  It uses a couple of undocumented or private API calls, so there's
no guarantee it'll work in future versions of OSX.

It's survived fairly rigorous use on Leopard (10.5.8), and the fetch functions
apparently work on Snow Leopard too.  The setting functions there are as-yet
untested.  If you find it works/doesn't work, please leave me a message
or add something to the
[issues](https://github.com/shabble/osx-space-id/issues)
page so I can update this statement with better data.

Currently there's no binary available.  If there's any interest, I can
build a Universal Binary and add it here somewhere. Message me if you
can't build it yourself.
