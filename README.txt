	   ===============================================
	   txblibs -- Troy's header only library functions
	   ===============================================


Sean Barrett and others show us the way to create and use relatively
small include files for library code. I recommend that you read the
FAQ on his https://github.com/nothings/stbs repository.

This repository holds my own bits of code that either aren't part of
the C standard or those versions don't work quite the way I want them
to.

Troy Brumley, blametroi@gmail.com, July 2024.

So let it be written,
So let it be done.



Installation and Use
--------------------

The headers to use are in the release/ sub-directory. They are built
by the makefile using buildhdr (https://github.com/BlameTroi/buildhdr).
The source for each header is a pair of files in the inc/ and src/
sub-directories. txbrand.h is built from inc/rand.h and src/rand.c,
and so on.

Include the appropriate headers in your project. Each header has a
preprocessor variable to control when the executable code will be
included. #define this in only one source file per linked target.



External Dependencies
---------------------

To build or work with these, you need nothing that isn't likely to
be already installed. If something is not already installed, it is
certainly available in all package managers.

 1. A C compiler (minimum c99, I compile with std=c18)

 2. CMake

 3. GNU Make

 4. ninja



License
-------

I can't see why anyone would want any of this, but it's all public
domain as far as I'm concerned, but you can use this software either
as public domain under the UNLICENSE or under the terms of the MIT
license.

(h/t to Sean Barrett at nothings.org for the most clear description of
usable licensing I've ever seen.)



Minunit
-------

In addition to using these libraries in my own work, I started writing
unit tests using Minunit. There are multiple descendants of John
Brewer's Minunit as found at https://jera.com/techinfo/jtns/jtn002.
Brewer's Minunit is minimal in the extreme but very instructive.

I want a bit more from a testing framework and the best I could find
was this version by David Siñuela Pastor siu.4coders@gmail.com at
https://github.com/siu/minunit.

I maintain my own fork at https://github.com/BlameTroi/minunit as I
tweak it to my preferences.

Included in source/inc is the current minunit.h used for this project.
The original is (c) 2022 by David Siñuela Pastor and released under
the MIT License. See https://github.com/aws/mit-0 for details.

Included in Minunit are two timing functions written by David Robert
Nadeau from http://NadeauSoftware.com/ and distributed under the
Creative Commons Attribution 3.0 Unported License. See
https://creativecommons.org/licenses/by/3.0/ for details.

As the unit tests are not something one would compile into application
binaries, Minunit has no impact on licensing or use of the libraries.

			    ==============
			    Project Layout
			    ==============



I use CMake and ninja to build the unit tests, and GNU Make to run
buildhdr to package up the release library headers. You will need to
create the build/ directory, it isn't part of the git repository.


.
├── LICENSE
├── MIT-LICENSE
├── Makefile
├── README.txt
├── build
├── doc
├── release
│   └── txb*.h
└── source
    ├── CMakeLists.txt
    ├── inc
    │   └── *.h
    ├── src
    │   └── *.c
    └── unit
        └── unit*.c


The build/ directory is not part of the git repository. Once you
create it, run the following to generate the ninja build scripts:

  cmake -G "Ninja Multi-Config" -S source -B build

Then to build the binaries for testing, run:

  cmake --build build --config (Release|Debug|RelWithDebInfo)

You may want to change or remove the section roughly around line 40
from "set(CMAKE_C_COMPILER..." through "add_link_options(...".

Depending upon which config you selected, the binaries for the
unit tests will be in the appropriate directory under build.



Header Files
------------

The bundled header libraries are stored in the release/ sub-directory.
Functions are in what I hope are sensible file groupings. Each header
can be included multiple times in your projects, but only one file
should define the implementation triggering macro:

#define <filename>_IMPLEMENTATION

The library headers are built using a library packer that combines the
development include and source together with guarding and triggering
preprocessor statements. The source for this utility is in the
source/src sub-directory.

These are the headers:

| File          | Description                                        |
|---------------+----------------------------------------------------|
| txbabort_if.h | common test and abort macro                        |
| txbda.h       | dynamic array                                      |
| txbdl.h       | doubly linked list                                 |
| txbkl.h       | keyed doubly linked list                           |
| txbkv.h       | key:value store that can have various backing      |
| txbmd5.h      | md-5 hash (credit to Bryce Wilson)                 |
| txbmisc.h     | "missing" functions such as min/max                |
| txbpat.h      | subset of regular expressions                      |
| txbpmute.h    | iterative permutation generator                    |
| txbpq.h       | priority queue                                     |
| txbqu.h       | queue                                              |
| txbrand.h     | random number support                              |
| txbrs.h       | string read stream                                 |
| txbsb.h       | string builder                                     |
| txbst.h       | stack                                              |
| txbstr.h      | split/tokenize and compare strings                 |

Some of these reference each other. You may need to include and define
the implementation trigger for a header that you don't explicitly
reference. 



Library Source
--------------

The source (.c) and basic header (.h) are stored in the source/src/
and source/inc/ sub-directories. Then a library include such as
release/txbpat.h is created by a header library packer utility (see
https://github.com/BlameTroi/buildhdr) from the src/pat.c and
inc/pat.h files.



Testing
-------

The test/ sub-directory contains test source for the unpacked versions
of the libraries: inc/da.h, src/da.c are included by unit/unitda.c.

Testing is done with my own version of minunit. My version is at
https://github.com/BlameTroi/minunit which is forked from
https://github.com/siu/minunit/blob/master/minunit.h.

			 ====================
			 Implementation Notes
			 ====================



Memory Management
-----------------

Generally the libraries are responsible for managing their own data.
An instance is created via a xx_create() function, and deleted via a
xx_destroy() function.

Instances are opaque struct typedefs and returned to the client as
typed pointers, but think of them as handles. Any control block that
the client code uses has a character tag used as a quick sanity check,
verifying that the correct instance type has been passed. If there is
an error, a message is printed on stderr and the program is terminated
by an abort().

Memory managed by the libraries is cleared when allocated and
overwritten with 0xfd (253) bytes before it is freed.



Client Data
-----------

Data structures such as the doubly linked list and priority queue have
control blocks. These are dynamically allocated and contain state for
an instance of the structure. If two priority queues are needed, each
will have its own control block.

I try to isolate the client code from the wiring inside the data
structure. While each item stored will require overhead for chain
pointers and such, the client only needs to deal with its own data,
referred to as value or payload in these libraries.

The payload may be any value that will fit inside a void * pointer.
This probably means it is really a pointer to one of the client's
structures.



Naming and Typedefs
-------------------

In the other header only libraries I've looked at, there is no
concensus on naming and prefixing. In C++ there are namespaces, but we
don't get those in C. I've taken a minimalist's approach and with very
few exceptions I use a two character prefix with an underscore: 'dl_'
for Doubly linked List, 'kv_' for Key:Value store, 'll_' for singly
Linked List, and so on.

I find snake_case to be more readable than either camelCase or
PascalCase.

All names that aren't normally uppercased outside these libraries
(example, MD5_) are lowercased.

Function names use the prefix and underscore for naming.

Most structures are defined as "typedef struct fred fred" instead of
the other alternatives. Their names are prefixed but do not have the
underscore. 'dlcb', kvcb', 'llcb', and so on.



Dealing with void *
-------------------

I find chains of dereferences and C's pointer syntax offensive at
best, but I realize that idiomatic usage is idiomatic. Pointers
aren't hard, but C obfuscates them.

These libraries deal with client data assumed to be provided as
anonymous pointers.

And here I bend the idiom a bit:

Depending on the context, clients are passing keys, values, or
payloads. These are all void * sized things and are typically
pointers.

In these instances, I use a typedef to hide the void * to avoid
(hah) constructs such as 'void **payload' and emphasize that
the libraries do not dereference these as pointers.

typedef void * pkey;
typedef void * pvalue;
typedef void * ppayload;



Error Handling and Reporting
----------------------------

Negative results (non errors) tend to return NULL or FALSE. Obviously
fatal errors such as NULL or invalid control block pointers will fail
with by printing a message to stderr and calling abort().

I had been using the assert macro but as release builds usually
disable assert by defining NDEBUG I created an abort_if macro.

abort_if(CONDITION, MESSAGE);

if (CONDITION) {
  fprintf(stderr, "abort: '%s' at %s:%d\n", MESSAGE, __FILE__, __LINE__);
  abort();
}

Where appropriate a library provides an xx_get_error(instance) which
returns a brief description of the last non-fatal error.


Comparator Functions
--------------------

Some libraries need to do comparisons on keys. The client must supply
a function via a pointer. The function has the same interface as the
comparators passed to qsort() and related functions:

int fn_cmp(const void *a, const void *b);

The return value is an integer less than zero if a comes before b,
zero if a equals b, and greater than zero if a comes after b.

			 ====================
			 Library Descriptions
			 ====================


Detailed API documentation can be found both the header and source
files for each library.


TXBABORT_IF.H
-------------

Defines the abort_if macro used throughout these libraries.


TXBDA.H
-------

A dynamically resizing array.


TXBDL.H
-------

A doubly linked list suitable for creating queues, stacks, or
iterators.


TXBST.H
-------

A simple stack. This is actually a thin API over txbdl.h.


TXBKL.H
-------

A keyed doubly linked list. Entries must have unique orderable keys.


TXBKV.H
-------

An initial pass at a key:value store that doesn't expose its
underlying data structures. The goal is to specify an appropriate
store based on application behavior.


TXBMD5.H
--------

I've needed an MD-5 hash for Advent of Code often enough that I
decided to put something in this libraries. I found Bryce Wilson's
lean and clean C implementation at https://github.com/Zunawe/md5-c
that he was kind enough to release with no restrictions or credit
required.

He gets a tip of the hat anyway.

I've massaged the source to fit my preferences, but he did the heavy
lifting.

Wilson provided functions to hide the messiness of the actual md5
calculation, and they should be used for most operations. the lower
level API functions are available as well, see inc/md5.h.


TXBMISC.H
---------

A hodgepodge of several small functions that fall into one or more of
the following categories:

* repplacements for functions with an API that make little sense to me
  or is too unlike what I'm used to from my prior work.

* functions that might be some library somewhere, but I'm not familiar
  enough with the various standards and feature test macro effects on
  them to bother finding them.

* functions I wrote because I'd rather do it myself. 

As these are libraries for personal projects, my definitions of digit,
character, and word characters is common US ASCII, that being
sufficient for my needs.


TXBPMUTE.H
----------

An iterative permutation generator.


TXBPAT.H
--------

A subset of regular expression pattern matching suitable for my needs.

The current omissions are:

* Grouping or capturing via ().

* Selection via |.

* Quantifiers using {,} syntax.

While I was heavily influenced by the chapters _Software Tools in
Pascal_ on pattern matching, searching, and editing, the code is not a
transliteration of the Pascal to C.

A first pass at filename globbing is pretty bad and will probably be
deleted.

There are some entry point declarations not explicitly exposed in the
header file for debugging and testing, but they are not marked static
in the implementation and can be declared as needed. These will be
removed at some point.


TXBPQ.H
-------

A priority queue, or more properly a priority deque. Items can be
retrieved from either the high or low end of the queue.


TXBRS.H
-------

A read stream on character strings.


TXBRAND.H
---------

Random number routines. These are not cryptographically safe and they
aren't meant to be. Either stdlib's rand() and arc4random() can be
selected. Seeding for rand() is supported.

The default generator is rand() for repeatable testing.

A Fisher-Yates array shuffle also available.


TXBSB.H
-------

A string builder.


TXBSTR.H
--------

Various character string functions that work the way I think they
should.


		     =============================
		     TO DO Items, Ideas, and Notes
		     =============================


pat:
---
- implement decompile_pattern
- implement {} quantifiers
- general refactoring
- (low priority) capture groups ()
- (low priority) or |


sb:
--
- non contiguous buffers.
