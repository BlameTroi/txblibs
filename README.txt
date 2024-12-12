	   ==============================================
	   txblibs -- Troy's Single File Header Libraries
	   ==============================================


Sean Barrett and others show us the way to create and use relatively
small include files for library code. I recommend that you read the
FAQ at his https://github.com/nothings/stbs repository.

This repository holds my own bits of code that either aren't part of
the C standard or those versions don't work quite the way I want them
to.

Troy Brumley, blametroi@gmail.com, November 2024.

So let it be written,
So let it be done.



Installation and Use
--------------------

The headers to use are in the source/inc sub-directory. Put them
somehwere in your C include path.

Include the appropriate headers in your project. Each header has a
preprocessor variable to control when the executable code will be
included. #define this in only one source file per linked target.



External Dependencies
---------------------

To build or work with these you need nothing that isn't likely to
be already installed. If it isn't, it is likely to be available
in your OS' package manager.

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
unit tests with Minunit. There are multiple descendants of John
Brewer's Minunit as found at https://jera.com/techinfo/jtns/jtn002.
Brewer's Minunit is minimal in the extreme but very instructive.

I wanted a bit more from a testing framework and the best I could find
was this version by David Siñuela Pastor siu.4coders@gmail.com at
https://github.com/siu/minunit.

I maintain my own fork at https://github.com/BlameTroi/minunit.

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



I use CMake and ninja to create dependency metadata for clang/clangd
and scripting to build the unit tests. CMakeFiles.txt describes the
dependencies and the compile_commands.json it produces is used file to
inform Emacs eglot & clangd of the build dependencies. You will need
to create the build/ directory, it isn't part of the git repository.


.
├── LICENSE
├── Makefile
├── README.txt
├── build
└── source
    ├── CMakeLists.txt
    ├── inc
    │   └── *.h
    └── unit
        └── unit*.c


Once build/ is created, run the following to generate the ninja build
scripts:

  cmake -G "Ninja Multi-Config" -S source -B build

Then to build the binaries for testing, run:

  cmake --build build --config (Release|Debug|RelWithDebInfo)

Depending upon which config you selected, the binaries for the
unit tests will be in the appropriate directory under build/.



Header Files
------------

Each header can be included multiple times in your projects, but only
one file should define the implementation triggering macro per binary
target:

#define <filename>_IMPLEMENTATION


These are the headers:

| File       | Description                                        |
|------------+----------------------------------------------------|
| txbabort.h | common test and abort macro                        |
| txbalist.h | an arraylist like collection                       |
| txballoc.h | a c/malloc-free trace to help find leaks           |
| txbdl.h    | doubly linked list                                 |
| txbkl.h    | keyed doubly linked list                           |
| txbkv.h    | key:value store that can have various backing      |
| txblog2.h  | a 32 bit unsigned integer log base 2 function      |
| txbmd5.h   | md-5 hash (credit to Bryce Wilson)                 |
| txbmisc.h  | "missing" functions such as min/max                |
| txbone.h   | unified data structure header library              |
| txbpat.h   | pattern matching with a regular expressions subset |
| txbpmute.h | iterative permutation generator                    |
| txbpq.h    | priority queue                                     |
| txbrand.h  | random number support                              |
| txbrs.h    | string read stream                                 |
| txbsb.h    | string builder                                     |
| txbstr.h   | split/tokenize and compare strings                 |
| txbwarn.h  | macro template for debugging traces                |

Some of these reference each other. You may need to define the
implementation trigger for a header that you don't explicitly
reference.

TXBST.H requires TXBDL.H
TXBPAT.H requires TXBSTR.H

Everything else requires TXBALLOC.H


Testing
-------

Testing is done with my version of minunit as described earlier in
this document. Most of the tests are self contained. Some will run
additional test cases if a file is specified as a command argument.

Examples:

./build/Debug/unitone 
./build/Debug/unitrs some_file.txt


			 ====================
			 Implementation Notes
			 ====================



Memory Management
-----------------

Libraries are responsible for managing their own data. A controlling
instance is created via an xx_create() or a make_one() function, and
deleted via an xx_destroy() or free_one() function.

Instances are opaque struct typedefs. While they are actually pointers,
they should be treated as handles.

Memory managed by the libraries is cleared when allocated and
overwritten with 0xfd (253) bytes before it is freed.

All library code uses wrappers over malloc, calloc, and free for
tracing and (future) pooled memory management. 


Client Data
-----------

Data structures such as the doubly linked list and priority queue have
instance handles. If two priority queues are needed, each will have
its own handle.

Client code is isolated the from the wiring in a data structure. While
each item of client data stored will require overhead for chain
pointers and such, the client only needs to deal with its own data,
referred to as key, value, or payload in these libraries.

The payload may be any value that will fit inside a void * pointer. The
libraries do not examine the payloads, but as they are often expected
to be pointers, NULL becomes a special case. See 'Dealing with void *'
later in this document for more details.

Client code should not use the macros and functions in txballoc.



Naming and Typedefs
-------------------

In the other header only libraries I've looked at, there is no
consensus on naming and prefixing. In C++ there are namespaces, but we
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
underscore. 'hda', hst', 'hll', and so on.



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


Detailed API documentation can be found in the inc/*.h files.


TXBABORT.H
----------

Defines the abort_if macro used throughout these libraries.


TXBALLOC.H
----------

Library memory management.


TXBALIST.H
----------

An array list like collection, initially intended as a
recursion accumulator as one would do with lists in
Lisp/Scheme/SML.


TXBDL.H
-------

A doubly linked list suitable for creating queues, stacks, or
iterators.


TXBKL.H
-------

A keyed doubly linked list. Entries must have unique orderable keys.


TXBKV.H
-------

An initial pass at a key:value store that doesn't expose its
underlying data structures. The goal is to specify an appropriate
store based on application behavior.


TXBLOG2.H
---------

A fast integer log2 function lifted from Bit Twiddling Hacks by Sean
Eron Anderson (seander@cs.stanford.edu).


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

* replacements for functions with an API that make little sense to me
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


TXBONE.H
--------

Some typical data structures: stacks, queues, dynamic arrays, and
more.


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

A first pass at filename globing is pretty bad and will probably be
deleted.

There are some entry point declarations not explicitly exposed in the
header file for debugging and testing, but they are not marked static
in the implementation and can be declared as needed. These will be
removed at some point.


TXBPQ.H
-------

A priority queue, or more properly a priority deque. Items can be
retrieved from either the high or low end of the queue.


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


TXBWARN.H
---------

Some macros for tracing/logging, usually for function arguments.
Intended more as a template.

			  =================
			  Oddities and Bugs
			  =================

This is weird but circumvented. In my Emacs configuration I get a
bogus error from eglot/clangd in the unit/mylib.c file. I say bogus
because the error message is:

mylibs.c ... 1 0 error e-f-b  clang [fatal_too_many_errors]:
Too many errors emitted, stopping now

That's it, that's the only diagnostic. It only shows up when both
txbkv.h and txbsb.h are included in unit/mylib.c. Remove either and
the error message is gone.

The build through CMake works fine with no errors. 

But if I don't compile txbrand.h as an object dependency for the unit
test targets, arc4random_uniform() is not defined. Defining macro
_DARWIN_C_SOURCE resolves that, but it should not be required.

I've examined the metadata for CMake and ninja and can not find a
difference. Same Emacs session, same shell session, different results.

A day of chasing both issues led to no explanation so I'm leaving it
as a circumvented issue. I'm not a fan of CMake and started using it
for clangd suppport in Emacs eglot. Emacs eglot with clangd requires
the compile_commands.json file produced when generating build scripts
to be useful.

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


all:
---
- begin deleting redundant data structure implementations as they are
  replaced by TXBONE.
  done: txbda txbll txbqu txbst
