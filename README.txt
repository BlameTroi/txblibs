	   ===============================================
	   txblibs -- Troy's header only library functions
	   ===============================================


Sean Barrett and others show us the way to create and use relatively
small include files for library code. I recommend that you read the
FAQ on his https://github.com/nothings/stbs repository.

This repository holds my own bits of code that either aren't part of
the C standard or don't work quite the way I want them to.

Troy Brumley, blametroi@gmail.com, July 2024.

So let it be written,
So let it be done.



Installation and Use
--------------------

The headers to use are in the release sub-directory. They are built by
the makefile using buildhdr (https://github.com/BlameTroi/buildhdr).
The source for each header is a pair of files in the inc and src
sub-directories. txbrand.h is built from inc/rand.h and src/rand.c,
and so on.

I keep all the release headers in my $HOME/.local/include/ directory
and add that to my $CPATH or in makefiles as needed.

Include the appropriate headers in your project. Each header has a
preprocessor variable to control when the executable code will be
included. #define this in only one source file per linked output.



External Dependencies
---------------------

Nothing that isn't already installed by anyone using these libraries.
If not already installed, they are available in most package managers.

 1. A C compiler (C99 support or above)

 2. CMake

 3. Gnu Make

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
unit tests using Minunit. There are multiple descendants of Minunit by
John Brewer as found at https://jera.com/techinfo/jtns/jtn002.
Brewer's Minunit is minimal in the extreme, and very instructive.

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

			    ==============
			    Project Layout
			    ==============



I use CMake and ninja to build the unit tests, and make to run my
buildhdr to package up the release library headers. Create the build
directory, it isn't part of the git repository.

.
├── LICENSE
├── MIT-LICENSE
├── Makefile
├── README.txt
├── build
├── doc
├── release
│   ├── txbda.h
│   ├── txbdl.h
│   ├── txbkl.h
│   ├── txbkv.h
│   ├── txbmd5.h
│   ├── txbmisc.h
│   ├── txbpat.h
│   ├── txbpmute.h
│   ├── txbpq.h
│   ├── txbqu.h
│   ├── txbrand.h
│   ├── txbrs.h
│   ├── txbsb.h
│   ├── txbst.h
│   └── txbstr.h
└── source
    ├── CMakeLists.txt
    ├── inc
    │   ├── da.h
    │   ├── dl.h
    │   ├── kl.h
    │   ├── kv.h
    │   ├── md5.h
    │   ├── minunit.h
    │   ├── misc.h
    │   ├── pat.h
    │   ├── pmute.h
    │   ├── pq.h
    │   ├── qu.h
    │   ├── rand.h
    │   ├── rs.h
    │   ├── sb.h
    │   ├── st.h
    │   └── str.h
    ├── src
    │   ├── da.c
    │   ├── dl.c
    │   ├── kl.c
    │   ├── kv.c
    │   ├── md5.c
    │   ├── minunit_example.c
    │   ├── misc.c
    │   ├── pat.c
    │   ├── pmute.c
    │   ├── pq.c
    │   ├── qu.c
    │   ├── rand.c
    │   ├── rs.c
    │   ├── sb.c
    │   ├── st.c
    │   └── str.c
    └── unit
        ├── unitda.c
        ├── unitdl.c
        ├── unitkl.c
        ├── unitkv.c
        ├── unitmd5.c
        ├── unitmisc.c
        ├── unitmute.c
        ├── unitpat.c
        ├── unitpq.c
        ├── unitqu.c
        ├── unitrand.c
        ├── unitrs.c
        ├── unitsb.c
        ├── unitst.c
        └── unitstr.c

The build directory is not part of the git repository. Once you create
it, run the following to generate the ninja build scripts:

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

<filename>_IMPLEMENTATION

The library headers are built using a library packer that combines the
development include and source together with guarding and triggering
preprocessor statements. The source for this utility is in the
source/src sub-directory.

These are the headers:

| File       | Description                                           |
|------------+-------------------------------------------------------|
| txbda.h    | dynamic array                                         |
| txbdl.h    | doubly linked list                                    |
| txbkl.h    | keyed doubly linked list                              |
| txbkv.h    | key:value store that can have various backing         |
| txbmd5.h   | md-5 hash (credit to Bryce Wilson)                    |
| txbmisc.h  | "missing" functions such as min/max                   |
| txbpat.h   | subset of regular expressions                         |
| txbpmute.h | iterative permutation generator                       |
| txbpq.h    | simple priority queue                                 |
| txbrand.h  | random number support                                 |
| txbrs.h    | string read stream                                    |
| txbsb.h    | string builder                                        |
| txbst.h    | a simple stack                                        |
| txbstr.h   | split/tokenize strings, dup_string, others            |

Some of these reference each other. You may need to include and define
the implementation trigger for a header that you don't explicitly
reference. 



Library Source
--------------

The source (.c) and basic header (.h) are stored in the source/src/
and source/inc/ sub-directories. Then a library include such as
txbpat.h is created by a header library packer utility (see
https://github.com/BlameTroi/buildhdr) from the src/pat.c and
inc/pat.h.



Testing
-------

The test/ sub-directory contains test source and build artifacts (.o)
for the unpacked versions of the libraries. These are still being
cleaned up, but ultimately each library will have a separate unit test
source file.

Testing is done with my own version of minunit. My version is at
https://github.com/BlameTroi/minunit which is forked from
https://github.com/siu/minunit/blob/master/minunit.h.

			 ====================
			 Implementation Notes
			 ====================



Memory Management
-----------------

Generally the libraries are responsible for both the allocation and
freeing of their own data. Function calls to create (allocate) and
destroy (free) an instance of a data structure are provided where
needed. The client is responsible for everything else.

Instances are opaque struct typedefs and returned to the client as
typed pointers, but think of them as handles. Any control block that
the client code uses has a character tag used as a quick sanity check,
verifying that the correct instance type has been passed. If there is
an error, the code fails via an assert.

Memory for instance data is cleared when allocated and overwritten
with 0xfd (253) bytes before it is freed.



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

The payload may be any value that will fit inside a void pointer, but
is expected to be a pointer to some client managed data.



Threading
---------

I added threading support before I had a clear need for it. I've since
removed it.



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

			 ====================
			 Library Descriptions
			 ====================


TXBABORT_IF.H
-------------

Defines the abort_if macro used throughout these libraries.



TXBDA.H
-------

A dynamically resizing array.

| Function          | Description                                    |
|-------------------+------------------------------------------------|
| da_create         | create new dynamic array                       |
| da_destroy        | release dynamic array owned resources          |
| da_put            | add an entry at a specific index               |
| da_get            | get the entry at a specific index              |
| da_length         | how many entries have potentially been added   |



TXBDL.H
-------

A doubly linked list suitable for creating queues, stacks, or
iteration.

| Function            | Description                                  |
|---------------------+----------------------------------------------|
| dl_create           | create a new doubly linked list.             |
| dl_destroy          | if empty, release any allocated memory for   |
|                     | the list.                                    |
| dl_empty            | is the list empty?                           |
| dl_count            | how many items are on the list?              |
| dl_reset            | bulk delete all entries on the list.         |
| dl_insert           | add a new item to the list.                  |
| dl_delete           | remove an item from the list.                |
| dl_update           | update an item on the list.                  |
| dl_get_first, _last | get the first or last item on the list.      |
| dl_get_next,        | read forward or backward through the list.   |
| _previous           |                                              |
| dl_get_error        | get a brief description of last error.       | 



TXBST.H
-------

A simple stack. This is actually a thin API over txbdl.h.

| Function          | Description                                    |
|-------------------+------------------------------------------------|
| st_create         | create a new stack                             |
| st_destroy        | release stack                                  |
| st_push           | push an item (pointer) on top of the stack     |
| st_pop            | remove and return the top item (pointer) from  |
|                   | the stack.                                     |
| st_peek           | return the top item (pointer) from the stack   |
|                   | but do not remove it.                          |
| st_empty          | predicates for checking stack state            |
| st_depth          | returns number of items on the stack.          |



TXBKL.H
-------

A keyed doubly linked list. Entries must have unique orderable keys.

| Function            | Description                                  |
|---------------------+----------------------------------------------|
| kl_create           | create a new doubly linked list.             |
| kl_destroy          | if empty, release any allocated memory for   |
|                     | the list.                                    |
| kl_empty            | is the list empty?                           |
| kl_count            | how many items are on the list?              |
| kl_reset            | bulk delete all entries on the list.         |
| kl_insert           | add a new item to the list.                  |
| kl_delete           | remove an item from the list.                |
| kl_update           | update an item on the list.                  |
| kl_get              | get a specific item by key from the list.    |
| kl_get_first        | get the first or last item on the list.      |
| kl_get_last         | get the first or last item on the list.      |
| kl_get_next,        | read forward or backward through the list.   |
| _previous           |                                              |
| kl_get_error        | get a brief description of last error.       | 



TXBKV.H
-------

An initial pass at a key:value store that doesn't expose its
underlying data structures. The goal is to specify an appropriate
storage structure for an application.

| Function          | Description                                    |
|-------------------+------------------------------------------------|
| kv_create         | create new key:value store                     |
| kv_destroy        | release key:value store resources              |
| kv_put            | add or update value for key in the store       |
| kv_get            | get value for key in store or NULL             |
| kv_delete         | remove a key and its value from the store      |
| kv_exists         | check if key exists in the store               |
| kv_empty          | is the store empty                             |
| kv_count          | how many items are in the store                |
| kv_keys           | return a null terminated list of keys or       |
| kv_values         | from the store, useful for iteration           |



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

| Function       | Description                                       |
|----------------+---------------------------------------------------|           
| md5_bytes      | these are the preferred functions to calculate    |
| md5_string     | md5 hashes for strings of bytes, a character      |
| md5_file       | string, or any file.                              |



TXBMISC.H
---------

A hodgepodge of several small functions that fall into one or more of
the following categories:

* functions with an API that makes little sense to me or is too unlike
  what I'm used to from my prior work.

* functions that might be some library somewhere, but I'm not familiar
  enough with the various standards and feature test macro effects on
  them to bother finding them.

* functions I just wrote because I'd rather do it myself. 

As these are libraries for personal projects, my definitions of digit,
character, and word characters is common US ASCII, that being
sufficient for my needs.


| Function       | Description                                       |
|----------------+---------------------------------------------------|           
| factors        | return an array of the factors of a long integer. |
| ?max ?min      | various flavors of min and max, inline not macro. |
| is_even        | is an integer even?                               |
| is_odd         | is an integer odd?                                |
| one_bits_in    | count one bits in an integer                      |
| is_digit       | is a character a decimal digit?                   |
| is_word_char   | is a character a word character (us ASCII letters |
|                | and the underscore _)?                            |
| is_lowercase   | a-z, us ASCII letters?                            |
| is_uppercase   | A-Z, us ASCII letters?                            |
| is_whitespace  | is the character a white space character (space,  |
|                | tab, newline)?                                    |
| is_punctuation | is the character an American English              |
|                | punctuation character?                            |
| is_control     | is a character an ASCII control character?        |
| is_bracketing  | is a character a bracketing or grouping           |
|                | character such as a parenthesis or brace?         |
| fn_cmp_xxx_asc | as needed, comparators for qsort like functions   |
| fn_cmp_xxx_dsc |                                                   |
| hex_pack       | convert a string of hex digits to bytes           |
| hex_unpack     | convert a string of bytes to hex digits           |



TXBPMUTE.H
----------

An iterative permutation generator.

| Function       | Description                                       |
|----------------+---------------------------------------------------|           
| permute_next   | permute the values in an integer array of a given |
|                | size in place.                                    |



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


Functions:

| Function          | Description                                    |
|-------------------+------------------------------------------------|
| convert_glob      | convert a filename glob to an equivalent       |
|                   | regular expression.                            |
| compile_pattern   | compile a regular expression for later match   |
|                   | processing.                                    |
| match             | does a string satisfy a pattern?               |
| glob_match        | does a string file name match using glob       |
|                   | rules?                                         |
| decompile_pattern | return a string representation of the pattern. |
| pattern_source    | return a read only copy of the original        |
|                   | pattern source.                                |



TXBPQ.H
-------

A priority queue.

| Function   | Description                                           |
|------------+-------------------------------------------------------|
| pq_empty   | is the queue empty?                                   |
| pq_put     | add an item to the queue with a specific priority.    |
| pq_get     | remove and return the highest priority item on the    |
|            | queue.                                                |
| pq_peek    | return the highest priority item on the queue, but    |
|            | do not remove it from the queue.                      |
| pq_create  | create a new empty priority queue.                    |
| pq_destroy | release system resources of an empty priority queue.  |
| pq_count   | how many items are in the queue?                      |



TXBRS.H
-------

A read stream on character strings (from file or memory).

| Function          | Description                                    |
|-------------------+------------------------------------------------|
| rs_create_string  | create a new reader on a copy of a string.     |
| ..._from_file     | create from entire contents of open file.      |
| rs_destroy_string | release all string read stream resources.      |
| rs_at_end         | has the last byte of the string been read?     |
| rs_position       | current character position in string.          |
| rs_length         | length of string.                              |
| rs_remaining      | characters left in string.                     |
| rs_rewind         | move to start of the string.                   |
| rs_seek           | move to a specific character position.         |
| rs_skip           | move some number of characters backward or     |
|                   | forward.                                       |
| rs_getc           | returns the next character from the string and |
|                   | advances.                                      |
| rs_ungetc         | back up one character, does not update string. |
| rs_peekc          | returns the next character from the string but |
|                   | does not advance.                              |
| rs_gets           | returns a string as fgets() would.             |



TXBRAND.H
---------

Random number routines. These are not cryptographically safe and they
aren't meant to be. Either stdlib's rand() and arc4random() can be
selected. Seeding for rand() is supported.

The default is rand() for repeatable testing.

| Function              | Description                                |
|-----------------------+--------------------------------------------|
| set_random_generator  | select between RAND_DEFAULT for rand() and |
|                       | RAND_RANDOM for arc4random().              |
| seed_random_generator | only for rand().                           |
| random_between        | return a pseudo random integer in an       |
|                       | inclusive range.                           |
| shuffle               | an implementation of the Fisher-Yates      |
|                       | shuffle for an array integers. Uses        |
|                       | the whichever set_random_generator has     |
|                       | been selected.                             |



TXBSB.H
-------

A string builder.

| Function          | Description                                    |
|-------------------+------------------------------------------------|
| sb_create         | create new empty string builder ...            |
| sb_create_blksize | ... with an explicit buffer size.              |
| sb_create_string  | default size with a starting string value.     |
| sb_reset          | empty the current string builder.              |
| sb_destroy        | release string builder resources.              |
| sb_length         | length of string stored in builder.            |
| sb_putc           | append character to builder.                   |
| sb_puts           | append string to builder.                      |
| sb_to_string      | returns a copy of the string in the builder.   |



TXBSTR.H
--------

Various character string functions that work the way I think they
should.

| Function       | Description                                       |
|----------------+---------------------------------------------------|           
| split_string   | splits a string on runs of any of the separators. |
| free_split     | split_string allocates two blocks, this frees     |
|                | them properly.                                    |
| dup_string     | today i learned strdup is not a standard, so i    |
|                | rolled my own.                                    |
| count_char     | how many times does a character appear in a       |
|                | string.                                           |
| pos_char       | index of a character in a string.                 |
| equal_string   | more readable predicates                          |
| less_than_.... |                                                   |
| greater_than_. |                                                   |

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


