===============================================
txblibs -- Troy's header only library functions
===============================================

Sean Barrett and others show us the way to create and use relatively
small include files for library code. I recommend that you read the
FAQ on his [[https://github.com/nothings/stb][stb]] repository at
GitHub.

This repository holds my own bits of code that either aren't part of
the C standard or don't work quite the way I want them to.

Troy Brumley, blametroi@gmail.com, July 2024.

So let it be written,
So let it be done.


====================
Installation and Use
====================

The headers to use are in the release subdirectory. They are built
from the makefile with my version of the library packer program,
found in the buildhdr subdirectory. The source for each header is a
pair of files in the inc and src subdirctories. txbabort.h is
built from inc/abort.h and src/abort.c, and so on.

I keep all the release headers in my $HOME/.local/include/ directory
and add that to my $CPATH in various makefiles.

Include the appropriate headers in your project. Each header has a
preprocessor variable to control when the executable code will be
included. #define this in only one source file per linked output.


=======
License
=======

It's all public domain as far as I'm concerned, but you can use this
software either as public domain under the unlicense or under the
terms of the MIT license.

(h/t to Sean Barrett at nothings.org for the most clear description of
usable licensing I've ever seen.)


==============
Project Layout
==============

A simple structure:

.                            [the project]
├── LICENSE
├── README.txt
├── buildhdr                 [source to the header packer]
│   ├── README.txt
│   ├── buildhdr.c
│   └── makefile
├── inc                      [headers for building libraries]
│   ├── abort.h
│   ├── listd.h
│   ├── misc.h
│   ├── pat.h
│   ├── pmute.h
│   ├── pq.h
│   └── str.h
├── makefile
├── release                  [release build of libraries]
│   ├── txbabort.h
│   ├── txblistd.h
│   ├── txbmisc.h
│   ├── txbpat.h
│   ├── txbpmute.h
│   ├── txbpq.h
│   └── txbstr.h
├── src                      [source for building libraries]
│   ├── abort.c
│   ├── listd.c
│   ├── misc.c
│   ├── pat.c
│   ├── pmute.c
│   ├── pq.c
│   └── str.c
└── test                     [source for testing]
    ├── makefile
    ├── testlibs.c
    ├── unitpat.c
    ├── unitpq.c
    └── unittest.c


Header Files
============

The complete header libraries are stored in the release/ subdirectory.
Functions are in what I hope are sensible file groupings. Each header
can be included multiple times in your projects, but only one file
should define the implementation triggering macro:

<filename>_IMPLEMENTATION.

The library headers are built using a library packer that combines the
development include and source together with guarding and triggering
preprocessor statements. The source for this utility is in the
buildhdr/ subdirectory.

These are the headers:

| File       | Description                                           |
|------------+-------------------------------------------------------|
| txbabort.h | abort and abort_if                                    |
| txblistd.h | doubly linked list                                    |
| txbmisc.h  | "missing" functions such as min/max                   |
| txbpat.h   | subset of regular expressions                         |
| txbpmute.h | iterative permutation generator                       |
| txbpq.h    | simple priority queue                                 |
| txbstr.h   | split and tokenize strings                            |

Some of these reference each other. At present you may need to include
and define the implementation trigger for a header that you don't
explicitly reference.


Library Source
==============

The source (.c) and basic header (.h) are stored in the src/ and inc/
subdirectories. The library include txbpat.h is created by a header
library packer utility (see the buildhdr/ subdirectory) from the
src/pat.c and inc/pat.h.


Testing
=======

The test/ subdirectory contains test source and build artifacts (.o)
for the unpacked versions of the libraries. These are still being
cleaned up, but ultimately each library will have a separate unit test
source file. Testing is done with my own version of minunit.


====================
Library Descriptions
====================


Implementation Notes
====================


Memory management:

Generally the libraries are responsible for both the allocation and
freeing of their own data. Function calls to create (allocate) and
destroy (free) an instance of a datastructure are provided where
needed. The client is responsible for everything else.


Client data:

Data structures such as the doubly linked list and priority queue have
control blocks. These are dynamically allocated and contain state for
an instance of the structure. If two priority queues are needed, each
will have its own control block.

I try to isolate the client code from the wiring inside the data
structure. While each item stored will require overhead for chain
pointers and such, the client only needs to deal with the payload.
This may be any value that will fit inside a void pointer, but is
typically actually a pointer to some client data.


Threading:

While I don't need thread support, it's available as an option on the
create functions for most of the data structure libraries. A pthread
mutex is used to lock and unlock the structure if threading is
enabled.


Error handling and reporting:

Error checking is generally limited. Obvious fatal errors such as NULL
or invalid control block pointers will fail with an assert. A to do
item is to replace the naked asserts with the abort and abort_if
macros and be more consistent in the reporting of errors.


TXBMISC.H
=========

This header library contains several small functions that I either
prefer to define myself, are not defined in standard libraries that I
know of yet, or are defined in libraries I don't use often.

These are libraries for personal projects and my definition of digit,
character, and word characters is US ASCII centric, that being
sufficient for my needs.

Depends on:

stdbool.h, stdlib.h, string.h

Functions:

| Function       | Description                                       |
|----------------+---------------------------------------------------|           
| rand_between   | return a pseudo random integer in an inclusive    |
|                | range.                                            |
| shuffle        | shuffle an array of pointers or long integers.    |
| factors        | return an array of the factors of a long integer. |
| ?max ?min      | various flavors of min and max, inline not macro. |
| is_even        | is an integer even?                               |
| is_odd         | is an integer odd?                                |
| is_digit       | is a character a decimal digit?                   |
| is_word_char   | is a character a word character (us ascii letters |
|                | and the underscore _)?                            |
| is_whitespace  | is the character a whitespace character (space,   |
|                | tab, newline)?                                    |
| is_punctuation | is the character an american english              |
|                | puncturation character?                           |
| is_control     | is a character an ascii control character?        |
| is_bracketing  | is a character a bracketing or grouping           |
|                | character such as a parenthesis or brace?         |


TXBPMUTE.H
==========

This library is meant to support permutation and combination. At
present it includes only one iterative permutation generator
function.

Depends on:

nothing.

Functions:

| Function       | Description                                       |
|----------------+---------------------------------------------------|           
| permute_next   | permute the values in an integer array of a given |
|                | size in place.                                    |


TXBSTR.H
========

This library mostly exists to split strings in a way that I like.

Depends on:

stdlib.h, string.h

Functions:

| Function       | Description                                       |
|----------------+---------------------------------------------------|           
| split_string   | splits a string on runs of any of the separators. |
| count_char     | how many times does a character appear in a       |
|                | string.                                           |
| pos_char       | index of a character in a string.                 |


txblistd functions
==================

This library provides a doubly linked list that may be optionally be
ordered. The client code presents payloads of for items in the list as
void pointers. If the list is ordered, a comparator function that
follows the strcmp/memcmp conventions must be provided. If the payload
is stored in allocated memory, a free function must be provided.

Depends on:

assert.h, stdbool.h, stdlib.h, 

Functions:

| Function            | Description                                  |
|---------------------+----------------------------------------------|
| reset_listd_control | resets/clears the list control block.        |
| free_all_items      | remove and optionally free memory for all    |
|                     | items on a list.                             |
| make_item           | allocate and initialize an unlinked item and |
|                     | attach its payload.                          |
| free_item           | free an unlinked item and its payload.       |
| find_item           | does an item with a matching payload key     |
|                     | exist in the list?                           |
| count_items         | how many items are on the list?              |
| add_item            | add unlinked item to the list.               |
| remove_item         | unlink item with a matching payload key from |
|                     | the list.                                    |
| next_item           | iterate through the items on the list.       |
| prev_item           | as next_item, but backwards.                 |


TXBPAT.H
========

This library provides a reasonable subset of regular expression
pattern matching.  Missing are:

- Grouping or capturing via ().
- Selection via |.
- Quantifiers using {,} syntax are not implemented.

There are some entry point declarations not explicitly exposed in the
header file for debugging and testing, but they are not marked static
in the implementation and can be declared as needed.

Depends on:

stdbool.h, stdlib.h, stdio.h, string.h, txbmisc.h, txbabort.h

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

glob_match and globbing are a bit of a pain. The implementation is
good enough for my current needs, but it may need more work as time
goes on.


TXBABORT.H
==========

This library defines two macros, abort(msg) and abort_if(condition,
msg), which call the functions do_abort and do_abort_if to report the
error message and then end the process via exit(EXIT_FAILURE).


TXBPQ.H
=======

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


====
TODO
====


all:

- use abort and abort_if


abort:

- improve error message support with snprintf


listd:

- review exposed api
- tag control blocks


misc:

- ?


pat:

- remove old debugging code
- implement decompile_pattern
- implement {} quantifiers
- general refactoring
- (low priority) capture groups ()
- (low priority) or |


pq:

- ?


str:

- add a free function for the block returned by split_string.