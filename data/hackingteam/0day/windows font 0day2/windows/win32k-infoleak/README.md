Summary.
========

Windows `win32k.sys` base address infoleak.

This is a standalone infoleak that is used to obtain the base address of
`win32k.sys` in memory. It can be further used, via the reading of various
pointers associated with `win32k.sys`, to reliably obtain the base address
of `ntoskrnl`, for a given build of `win32k.sys`.

Tested on Windows 8.1 fully-patched (as of 28 Jan 2015).


Compiling the infoleak.
=======================

The exploit is tested with the following compiler:

`x86_64-w64-mingw32-g++ (rev2, Built by MinGW-W64 project) 4.8.1`

The compiler may be installed using the provided `mingw-w64-install.exe`. 

The installer was originally obtained from SourceForge (refer to src below), and is provided for convenience. No modifications were made.

(src: http://sourceforge.net/projects/mingw-w64/files/latest/download)

A convenience script, `make.bat`, is provided that builds the infoleak
automatically.
