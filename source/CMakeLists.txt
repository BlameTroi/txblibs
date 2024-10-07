# Set the minimun CMake version required by the project

cmake_minimum_required(VERSION 3.5)

# Set a name for the project

project(txblibs)

#
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_C_FLAGS "-Wall -Werror -g3 -pedantic-errors")



# Create an executable with the source code file

add_executable(unitda "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitda.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/da.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/rand.c")
target_include_directories(unitda PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_executable(unitdl "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitdl.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/dl.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/misc.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/rand.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/str.c")
target_include_directories(unitdl PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_executable(unitkl "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitkl.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/kl.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/rand.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/str.c")
target_include_directories(unitkl PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_executable(unitkv "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitkv.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/kv.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/rand.c")
target_include_directories(unitkv PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_executable(unitmd5 "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitmd5.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/md5.c")
target_include_directories(unitmd5 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_executable(unitmisc "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitmisc.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/misc.c")
target_include_directories(unitmisc PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_executable(unitmute "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitmute.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/pmute.c")
target_include_directories(unitmute PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_executable(unitpat "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitpat.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/pat.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/misc.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/str.c")
target_include_directories(unitpat PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_executable(unitpq "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitpq.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/pq.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/rand.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/str.c")
target_include_directories(unitpq PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_executable(unitqu "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitqu.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/str.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/qu.c")
target_include_directories(unitqu PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_executable(unitrand "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitrand.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/rand.c")
target_include_directories(unitrand PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_executable(unitrs "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitrs.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/str.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/rs.c")
target_include_directories(unitrs PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_executable(unitsb "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitsb.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/sb.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/str.c")
target_include_directories(unitsb PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_executable(unitst "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitst.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/st.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/dl.c")
target_include_directories(unitst PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_executable(unitstr "${CMAKE_CURRENT_SOURCE_DIR}/unit/unitstr.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/str.c")
target_include_directories(unitstr PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

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
