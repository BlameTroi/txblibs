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
  VERSION 0.0.0.1
  DESCRIPTION "single header c libraries mostly for my personal use"
  HOMEPAGE_URL "https://github.com/BlameTroi/txblibs"
  LANGUAGES C
)

# i'm picky about compiler and options
#
# TO DO how do i change these for the other configurations
#
# something like the following is wrong, i'm using configs not builds,
# but i don't yet know if CMAKE_CONFIG_TYPE is usable this way.
#
# if(CMAKE_BUILD_TYPE MATCHES "Debug")
#   add_compile_options( -Wall --pedantic-errors -Werror -G3 -fsanitize=address -std=c18 )
# elseif(CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
#   add_compile_options( -Wall --pedantic-errors -Werror -G -std=c18 )
# elseif(CMAKE_BUILD_TYPE MATCHES "Release")
#   add_compile_options( -Wall --pedantic-errors -Werror -O2 -std=c18 )
# endif()
#
# testing without tweaking produces binaries of different sizes so it looks
# as if we're moving in the right direction.

set(
  CMAKE_C_COMPILER "clang"
)
add_compile_options(
  -Wall
  --pedantic-errors
  -Werror
  -g3
  -fsanitize=address
  -std=c18
)
add_link_options(
  -fsanitize=address
)

# the build done here is for compilation and testing with
# minunit. the executables are all command line programs.

# the following seems redundant but i think of it like a
# linkdeck from my mainframe days. declarative and the
# repetition isn't very expensive. the gnu make scripting
# to do this on the fly was more hassle than it was worth.

add_executable(unitda "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitda.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/da.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/rand.c")
target_include_directories(unitda PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")

add_executable(unitdl "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitdl.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/dl.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/misc.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/rand.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/str.c")
target_include_directories(unitdl PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")

add_executable(unitkl "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitkl.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/kl.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/rand.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/str.c")
target_include_directories(unitkl PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")

add_executable(unitkv "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitkv.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/kv.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/rand.c")
target_include_directories(unitkv PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")

add_executable(unitmd5 "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitmd5.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/md5.c")
target_include_directories(unitmd5 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")

add_executable(unitmisc "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitmisc.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/misc.c")
target_include_directories(unitmisc PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")

add_executable(unitmute "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitmute.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/pmute.c")
target_include_directories(unitmute PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")

add_executable(unitpat "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitpat.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/pat.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/misc.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/str.c")
target_include_directories(unitpat PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")

add_executable(unitpq "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitpq.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/pq.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/rand.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/str.c")
target_include_directories(unitpq PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")

add_executable(unitqu "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitqu.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/str.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/qu.c")
target_include_directories(unitqu PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")

add_executable(unitrand "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitrand.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/rand.c")
target_include_directories(unitrand PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")

add_executable(unitrs "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitrs.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/str.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/rs.c")
target_include_directories(unitrs PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")

add_executable(unitsb "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitsb.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/sb.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/str.c")
target_include_directories(unitsb PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")

add_executable(unitst "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitst.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/st.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/dl.c")
target_include_directories(unitst PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")

add_executable(unitstr "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitstr.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/str.c")
target_include_directories(unitstr PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")

# add_library(txblibs OBJECT
#   "${CMAKE_CURRENT_SOURCE_DIR}/src/da.c"
#   "${CMAKE_CURRENT_SOURCE_DIR}/src/da.c"
#   "${CMAKE_CURRENT_SOURCE_DIR}/src/dl.c"
#   "${CMAKE_CURRENT_SOURCE_DIR}/src/kl.c"
#   "${CMAKE_CURRENT_SOURCE_DIR}/src/kv.c"
#   "${CMAKE_CURRENT_SOURCE_DIR}/src/md5.c"
#   "${CMAKE_CURRENT_SOURCE_DIR}/src/misc.c"
#   "${CMAKE_CURRENT_SOURCE_DIR}/src/pat.c"
#   "${CMAKE_CURRENT_SOURCE_DIR}/src/pmute.c"
#   "${CMAKE_CURRENT_SOURCE_DIR}/src/pq.c"
#   "${CMAKE_CURRENT_SOURCE_DIR}/src/qu.c"
#   "${CMAKE_CURRENT_SOURCE_DIR}/src/rand.c"
#   "${CMAKE_CURRENT_SOURCE_DIR}/src/rs.c"
#   "${CMAKE_CURRENT_SOURCE_DIR}/src/sb.c"
#   "${CMAKE_CURRENT_SOURCE_DIR}/src/st.c"
#   "${CMAKE_CURRENT_SOURCE_DIR}/src/str.c")
