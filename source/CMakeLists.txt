
# CMakeLists.txt for txblibs
#
# cmake -G "Ninja Multi-Config" -S source -B build
# cmake --build build --config Release or Debug or RelWithDebInfo ...
#
# this builds the unit tests for the libraries. to generate the
# release includes use Makefile in the root directory to run
# buildhdr which bundles up the xx.h and xx.c files into txbxx.h
# for client use.

cmake_minimum_required(VERSION 3.5)

project(
  TXBLIBS
  VERSION 1.3.0.1
  DESCRIPTION "single header c libraries mostly for my personal use"
  HOMEPAGE_URL "https://github.com/BlameTroi/txblibs"
  LANGUAGES C
)

# i'm picky about compiler and options
#
# cmake and/or ninja default these so that the debug configurations
# include -g while the release configuration removes -g and adds
# -DNDEBUG -O3.
#
# i use assert for error handling so for compiled code that counts
# on assert i need to manage NDEBUG better.
#
# the only executables produced by this project are the minunit
# example, the unit tests, and buildhdr. buildhdr works well
# enough that it won't miss the asserts and i'll figure out
# a proper way to handle NDEBUG.

set(
  CMAKE_C_COMPILER "clang"
)

# cmake is a moving target, and the compile options conditionals via
# if() don't work (if they ever did). the following passes options to
# each build configuration properly.
#
# first, create options variable strings for each configuration.
#
# then, for each target, specify:
#
# target_<step>_options(generator_expression:replacement)
#
# for each possible target configuration using generator expressions
# for your configurations.

# _DARWIN_C_SOURCE is required for arc4random* functions, but not always.
# confusing but i've spent too much time on it.

set(MY_RELEASE_OPTIONS "-Wall -Werror -pedantic-errors -std=c18")
set(MY_RELWITHDEBINFO_OPTIONS "-Wall -Werror -pedantic-errors -std=c18 -g")
set(MY_DEBUG_OPTIONS "-Wall -Werror -pedantic-errors -std=c18 -g -fsanitize=address")
set(MY_DEBUG_LINK_OPTIONS "-fsanitize=address")

add_executable(unitone "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitone.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txballoc.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbstr.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/txbone.c")
target_include_directories(unitone PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
target_link_options(unitone PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_LINK_OPTIONS}>")
target_compile_options(unitone PUBLIC "$<$<CONFIG:RELWITHDEBINFO>:SHELL:${MY_REL_DEB_OPTIONS}>")
target_compile_options(unitone PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_OPTIONS}>")
target_compile_options(unitone PUBLIC "$<$<CONFIG:RELEASE>:SHELL:${MY_RELEASE_OPTIONS}>")

add_executable(unitbtree "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitbtree.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txballoc.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbalist.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbrand.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txblog2.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/txbtree.c")
target_include_directories(unitbtree PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
target_link_options(unitbtree PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_LINK_OPTIONS}>")
target_compile_options(unitbtree PUBLIC "$<$<CONFIG:RELWITHDEBINFO>:SHELL:${MY_REL_DEB_OPTIONS}>")
target_compile_options(unitbtree PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_OPTIONS}>")
target_compile_options(unitbtree PUBLIC "$<$<CONFIG:RELEASE>:SHELL:${MY_RELEASE_OPTIONS}>")

add_executable(unitalist "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitalist.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txballoc.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbalist.c")
target_include_directories(unitalist PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
target_include_directories(unitalist PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/txblibs")
target_link_options(unitalist PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_LINK_OPTIONS}>")
target_compile_options(unitalist PUBLIC "$<$<CONFIG:RELWITHDEBINFO>:SHELL:${MY_REL_DEB_OPTIONS}>")
target_compile_options(unitalist PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_OPTIONS}>")
target_compile_options(unitalist PUBLIC "$<$<CONFIG:RELEASE>:SHELL:${MY_RELEASE_OPTIONS}>")

add_executable(unitda "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitda.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbrand.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txballoc.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbmisc.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbstr.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/txbone.c")
target_include_directories(unitda PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
target_link_options(unitda PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_LINK_OPTIONS}>")
target_compile_options(unitda PUBLIC "$<$<CONFIG:RELWITHDEBINFO>:SHELL:${MY_REL_DEB_OPTIONS}>")
target_compile_options(unitda PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_OPTIONS}>")
target_compile_options(unitda PUBLIC "$<$<CONFIG:RELEASE>:SHELL:${MY_RELEASE_OPTIONS}>")

add_executable(unitdl "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitdl.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbrand.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txballoc.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbmisc.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbstr.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbdl.c")
target_include_directories(unitdl PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
target_link_options(unitdl PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_LINK_OPTIONS}>")
target_compile_options(unitdl PUBLIC "$<$<CONFIG:RELWITHDEBINFO>:SHELL:${MY_REL_DEB_OPTIONS}>")
target_compile_options(unitdl PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_OPTIONS}>")
target_compile_options(unitdl PUBLIC "$<$<CONFIG:RELEASE>:SHELL:${MY_RELEASE_OPTIONS}>")

add_executable(unitkl "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitkl.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbrand.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txballoc.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbstr.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbkl.c")
target_include_directories(unitkl PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
target_link_options(unitkl PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_LINK_OPTIONS}>")
target_compile_options(unitkl PUBLIC "$<$<CONFIG:RELWITHDEBINFO>:SHELL:${MY_REL_DEB_OPTIONS}>")
target_compile_options(unitkl PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_OPTIONS}>")
target_compile_options(unitkl PUBLIC "$<$<CONFIG:RELEASE>:SHELL:${MY_RELEASE_OPTIONS}>")

add_executable(unitkv "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitkv.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txballoc.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbmisc.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbstr.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbrand.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbkv.c")
target_include_directories(unitkv PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
target_link_options(unitkv PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_LINK_OPTIONS}>")
target_compile_options(unitkv PUBLIC "$<$<CONFIG:RELWITHDEBINFO>:SHELL:${MY_REL_DEB_OPTIONS}>")
target_compile_options(unitkv PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_OPTIONS}>")
target_compile_options(unitkv PUBLIC "$<$<CONFIG:RELEASE>:SHELL:${MY_RELEASE_OPTIONS}>")

add_executable(unitll "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitll.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txballoc.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbmisc.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbstr.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/txbone.c")
target_include_directories(unitll PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
target_link_options(unitll PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_LINK_OPTIONS}>")
target_compile_options(unitll PUBLIC "$<$<CONFIG:RELWITHDEBINFO>:SHELL:${MY_REL_DEB_OPTIONS}>")
target_compile_options(unitll PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_OPTIONS}>")
target_compile_options(unitll PUBLIC "$<$<CONFIG:RELEASE>:SHELL:${MY_RELEASE_OPTIONS}>")

add_executable(unitlog2 "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitlog2.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txballoc.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbrand.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txblog2.c")
target_include_directories(unitlog2 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
target_link_options(unitlog2 PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_LINK_OPTIONS}>")
target_compile_options(unitlog2 PUBLIC "$<$<CONFIG:RELWITHDEBINFO>:SHELL:${MY_REL_DEB_OPTIONS}>")
target_compile_options(unitlog2 PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_OPTIONS}>")
target_compile_options(unitlog2 PUBLIC "$<$<CONFIG:RELEASE>:SHELL:${MY_RELEASE_OPTIONS}>")

add_executable(unitmd5 "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitmd5.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbmd5.c")
target_include_directories(unitmd5 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
target_link_options(unitmd5 PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_LINK_OPTIONS}>")
target_compile_options(unitmd5 PUBLIC "$<$<CONFIG:RELWITHDEBINFO>:SHELL:${MY_REL_DEB_OPTIONS}>")
target_compile_options(unitmd5 PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_OPTIONS}>")
target_compile_options(unitmd5 PUBLIC "$<$<CONFIG:RELEASE>:SHELL:${MY_RELEASE_OPTIONS}>")

add_executable(unitmisc "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitmisc.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbmisc.c")
target_include_directories(unitmisc PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
target_link_options(unitmisc PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_LINK_OPTIONS}>")
target_compile_options(unitmisc PUBLIC "$<$<CONFIG:RELWITHDEBINFO>:SHELL:${MY_REL_DEB_OPTIONS}>")
target_compile_options(unitmisc PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_OPTIONS}>")
target_compile_options(unitmisc PUBLIC "$<$<CONFIG:RELEASE>:SHELL:${MY_RELEASE_OPTIONS}>")

add_executable(unitpmute "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitpmute.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbpmute.c")
target_include_directories(unitpmute PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
target_link_options(unitpmute PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_LINK_OPTIONS}>")
target_compile_options(unitpmute PUBLIC "$<$<CONFIG:RELWITHDEBINFO>:SHELL:${MY_REL_DEB_OPTIONS}>")
target_compile_options(unitpmute PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_OPTIONS}>")
target_compile_options(unitpmute PUBLIC "$<$<CONFIG:RELEASE>:SHELL:${MY_RELEASE_OPTIONS}>")

add_executable(unitpat "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitpat.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txballoc.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbmisc.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbstr.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbpat.c")
target_include_directories(unitpat PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
target_link_options(unitpat PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_LINK_OPTIONS}>")
target_compile_options(unitpat PUBLIC "$<$<CONFIG:RELWITHDEBINFO>:SHELL:${MY_REL_DEB_OPTIONS}>")
target_compile_options(unitpat PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_OPTIONS}>")
target_compile_options(unitpat PUBLIC "$<$<CONFIG:RELEASE>:SHELL:${MY_RELEASE_OPTIONS}>")

add_executable(unitpq "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitpq.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txballoc.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbrand.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbmisc.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbstr.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbpq.c")
target_include_directories(unitpq PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
target_link_options(unitpq PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_LINK_OPTIONS}>")
target_compile_options(unitpq PUBLIC "$<$<CONFIG:RELWITHDEBINFO>:SHELL:${MY_REL_DEB_OPTIONS}>")
target_compile_options(unitpq PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_OPTIONS}>")
target_compile_options(unitpq PUBLIC "$<$<CONFIG:RELEASE>:SHELL:${MY_RELEASE_OPTIONS}>")

add_executable(unitqu "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitqu.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txballoc.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbstr.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/txbone.c")
target_include_directories(unitqu PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
target_link_options(unitqu PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_LINK_OPTIONS}>")
target_compile_options(unitqu PUBLIC "$<$<CONFIG:RELWITHDEBINFO>:SHELL:${MY_REL_DEB_OPTIONS}>")
target_compile_options(unitqu PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_OPTIONS}>")
target_compile_options(unitqu PUBLIC "$<$<CONFIG:RELEASE>:SHELL:${MY_RELEASE_OPTIONS}>")

add_executable(unitrand "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitrand.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbmisc.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbrand.c")
target_include_directories(unitrand PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
target_link_options(unitrand PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_LINK_OPTIONS}>")
target_compile_options(unitrand PUBLIC "$<$<CONFIG:RELWITHDEBINFO>:SHELL:${MY_REL_DEB_OPTIONS}>")
target_compile_options(unitrand PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_OPTIONS}>")
target_compile_options(unitrand PUBLIC "$<$<CONFIG:RELEASE>:SHELL:${MY_RELEASE_OPTIONS}>")

add_executable(unitrs "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitrs.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txballoc.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbstr.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbrs.c")
target_include_directories(unitrs PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
target_link_options(unitrs PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_LINK_OPTIONS}>")
target_compile_options(unitrs PUBLIC "$<$<CONFIG:RELWITHDEBINFO>:SHELL:${MY_REL_DEB_OPTIONS}>")
target_compile_options(unitrs PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_OPTIONS}>")
target_compile_options(unitrs PUBLIC "$<$<CONFIG:RELEASE>:SHELL:${MY_RELEASE_OPTIONS}>")

add_executable(unitsb "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitsb.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txballoc.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbstr.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbsb.c")
target_include_directories(unitsb PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
target_link_options(unitsb PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_LINK_OPTIONS}>")
target_compile_options(unitsb PUBLIC "$<$<CONFIG:RELWITHDEBINFO>:SHELL:${MY_REL_DEB_OPTIONS}>")
target_compile_options(unitsb PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_OPTIONS}>")
target_compile_options(unitsb PUBLIC "$<$<CONFIG:RELEASE>:SHELL:${MY_RELEASE_OPTIONS}>")

add_executable(unitst "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitst.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txballoc.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/txbone.c")
target_include_directories(unitst PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
target_link_options(unitst PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_LINK_OPTIONS}>")
target_compile_options(unitst PUBLIC "$<$<CONFIG:RELWITHDEBINFO>:SHELL:${MY_REL_DEB_OPTIONS}>")
target_compile_options(unitst PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_OPTIONS}>")
target_compile_options(unitst PUBLIC "$<$<CONFIG:RELEASE>:SHELL:${MY_RELEASE_OPTIONS}>")

add_executable(unitstr "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitstr.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txballoc.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_txbstr.c")
target_include_directories(unitstr PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
target_link_options(unitstr PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_LINK_OPTIONS}>")
target_compile_options(unitstr PUBLIC "$<$<CONFIG:RELWITHDEBINFO>:SHELL:${MY_REL_DEB_OPTIONS}>")
target_compile_options(unitstr PUBLIC "$<$<CONFIG:DEBUG>:SHELL:${MY_DEBUG_OPTIONS}>")
target_compile_options(unitstr PUBLIC "$<$<CONFIG:RELEASE>:SHELL:${MY_RELEASE_OPTIONS}>")
