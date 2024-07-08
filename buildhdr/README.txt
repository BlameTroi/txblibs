==============================================================
BUILDHDR -- a packager to build single file C header libraries
==============================================================


BACKGROUND AND CREDITS
======================

This utility was inspired by Apoorva Joshi's single_header_packer.py,
which I learned about from Micha Mettke's Nuklear immediate mode GUI
library (https://github.com/Immediate-Mode-UI/Nuklear). The original
Python utility is in the public domain under the unlicense.


MOTIVATION
==========

I do my editing in Emacs. While eglot and tree-sitter and such work
well, they aren't built for editing a single file header library. The
imenu list is thrown off by the nested defines, as is xref goto.

I decided to write a C version for my own use and to keep me busy.
There's nothing really new here, but I did roll my own regular
expressions and file globbing support.


AUTHORSHIP, LICENSING, AND COPYRIGHT
====================================

Some or all of the code in this project is by Troy Brumley
(BlameTroi). Prior work preserves its original licensing and author
copyrights. Anything I write all public domain as far as I'm
concerned, but you can use this software either as public domain under
the unlicense or under the terms of the MIT license.

(h/t to Sean Barrett at nothings.org for the most clear description of
usable licensing I've ever seen.)

Troy Brumley
blametroi@gmail.com

So let it be written,
So let it be done.


USAGE
=====

With the single header packer you can build a single header library
from working 'live' code. When developing a new header library, I
partition the code into inc/lib.h and src/lib.c files. The source
files includes their headers via "#include ../inc/lib.h". Files listed
in --priv and will have their #includes of files in --pub suppressed.

Buildhdr can be used to combine these files and wrap them in pre-
processor #if(n)def blocks, plus add any licensing or documentation
blocks.

Input files should be provided as command line arguments. Output is
written to standard output.


Command Format
--------------

     buildhdr --macro <macro>
             [--intro <files>]
              --pub <files>
              --priv <files>
             [--outro <files>]

Where <files> is space separated list of files. e.g. --pub defs/*.h
str/*.h --priv inc/*.h defs/*.c str/*.c

The --intro and --outro files are optional. If provided, they are
expected to be plain text files. In particular, embedded C comments
are not allowed.

Files in --pub should contain the declarations that a client program
needs to use the library.

Files in --priv should contain the actual code for the library. Their
inclusion in a client program compilation is controlled by the
preprocessor macro <macro>_IMPLEMENTATION.


Command Output
--------------

The resulting code is packed as follows:

     /*
     [intro file contents]
     */

     #ifndef <macro>_SINGLE_HEADER
     #define <macro>_SINGLE_HEADER
     [public header file contents]
     #endif /* <macro>_SINGLE_HEADER */

     #ifdef <macro>_IMPLEMENTATION
     #undef <macro>_IMPLEMENTATION
     [private header and source file contents]
     #endif /* <macro>_IMPLEMENTATION */

     /*
     [outro file contents]
     */


Differences from the Original Python Version
--------------------------------------------

Unlike the original Python version, I prefer to let the shell handle
file globbing and environment variable expansion. That's what it's
there for. Because of this, filenames are space separated instead of
comma separated. Spaces in filenames are evil, but if you must have
them they will work if you escape or quote them.

I thought about adding a standard licenses option but that can be
handled with --intro and --outro.

I edit my code with Emacs and use form feeds as a visual separator.
They are removed from files in --priv and --pub.

Finally, I added an #undef <macro>_IMPLEMENTATION around the --priv
block to avoid accidental dual generation of the library.


FILES AND INSTALLATION
======================

The program is one file, buildhdr.c, but it depends on my abort, misc,
str, and pat libraries (https://github.com/BlameTroi/txblibs). A
standard makefile is provided. Put the binary somewhere in your path.
