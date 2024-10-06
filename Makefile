# Makefile
#
# still trying to get out of source builds figured out
#
# this scripting comes from:
#
# https://riptutorial.com/makefile/example/21376/building-from-different-source-folders-to-different-target-folders
#
# which addresses multiple source directories, everything else i've found is too
# terse for my present level of make understanding or single source.
#
# projdir is git project root
# sourcedir could be a directory holding other sources, and indeed it will be
# builddir holds this makefile with output targets below it
#
# Set project directory one level above of Makefile directory. $(CURDIR) is a GNU make variable containing the path to the current working directory
# txb so make file goes in source dir? no, build dir, with output under it in subdirs
# .
# ├── LICENSE
# ├── Makefile
# ├── README.txt
# ├── build
# │   ├── debug
# │   ├── release
# │   └── unit
# ├── doc
# │   └── void.txt
# ├── release
# ├── side-notes.txt
# ├── source
# │   ├── src
# │   └── unit
# │       └── unitstr.c
# └── tree.txt

.POSIX

# trying to keep Makefile in sources

PROJDIR := $(realpath $(CURDIR)/..)
SOURCEDIR := $(PROJDIR)/source
BUILDDIR := $(PROJDIR)/build

# Name of the final executable
# txb not for this particular make
TARGET = myApp.exe

# Decide whether the commands will be shwon or not
VERBOSE = TRUE

# Create the list of directories
DIRS = src unit
SOURCEDIRS = $(foreach dir, $(DIRS), $(addprefix $(SOURCEDIR)/, $(dir)))
TARGETDIRS = $(foreach dir, $(DIRS), $(addprefix $(BUILDDIR)/, $(dir)))

# Generate the GCC includes parameters by adding -I before each source folder
INCLUDES = $(foreach dir, $(SOURCEDIRS), $(addprefix -I, $(dir)))

# Add this list to VPATH, the place make will look for the source files
VPATH = $(SOURCEDIRS)

# Create a list of *.c sources in DIRS
SOURCES = $(foreach dir,$(SOURCEDIRS),$(wildcard $(dir)/*.c))

# Define objects for all sources
OBJS := $(subst $(SOURCEDIR),$(BUILDDIR),$(SOURCES:.c=.o))

# Define dependencies files for all objects
DEPS = $(OBJS:.o=.d)

# my standards for c
CC = clang
# CFLAGS = -Wall -o 2 -std=c18 --pedantic-errors
CFLAGS = -fsanitize=address -Wall -g3 -o 0 -std=c18 --pedantic-errors
LDFLAGS =
# LDFLAGS = -Lm

# OS specific part
ifeq ($(OS),Windows_NT)
	RM = del /F /Q
	RMDIR = -RMDIR /S /Q
	MKDIR = -mkdir
	ERRIGNORE = 2>NUL || true
	SEP=\\
else
	RM = rm -rf
	RMDIR = rm -rf
	MKDIR = mkdir -p
	ERRIGNORE = 2>/dev/null
	SEP=/
endif

# Remove space after separator
PSEP = $(strip $(SEP))

# Hide or not the calls depending of VERBOSE
ifeq ($(VERBOSE),TRUE)
	HIDE =
else
	HIDE = @
endif

# Define the function that will generate each rule
define generateRules
$(1)/%.o: %.c
	@echo Building $$@
	$(HIDE)$(CC) -c $$(INCLUDES) -o $$(subst /,$$(PSEP),$$@) $$(subst /,$$(PSEP),$$<) -MMD
endef

.PHONY: all clean directories

all: directories $(TARGET)

$(TARGET): $(OBJS)
	$(HIDE)echo Linking $@
	$(HIDE)$(CC) $(OBJS) -o $(TARGET)

# Include dependencies
-include $(DEPS)

# Generate rules
$(foreach targetdir, $(TARGETDIRS), $(eval $(call generateRules, $(targetdir))))

directories:
	$(HIDE)$(MKDIR) $(subst /,$(PSEP),$(TARGETDIRS)) $(ERRIGNORE)

# Remove all objects, dependencies and executable files generated during the build
clean:
	$(HIDE)$(RMDIR) $(subst /,$(PSEP),$(TARGETDIRS)) $(ERRIGNORE)
	$(HIDE)$(RM) $(TARGET) $(ERRIGNORE)
	@echo Cleaning done !

# How to use this Makefile To adapt this Makefile to your project you have to :

# Change the TARGET variable to match your target name
# Change the name of the Sources and Build folders in SOURCEDIR and BUILDDIR
# Change the verbosity level of the Makefile in the Makefile itself or in make call
# Change the name of the folders in DIRS to match your sources and build folders
# If required, change the compiler and the flags


# # Makefile for txblibs
# # expects buildhdr in the path

# .POSIX:

# .PHONY:  clean all unit test build install

# # my standards for c
# CC = clang
# CFLAGS = -Wall -o 2 -std=c18 --pedantic-errors
# DFLAGS = -fsanitize=address -Wall -g3 -o 0 -std=c18 --pedantic-errors
# LDFLAGS =
# # LDFLAGS = -Lm

# # additional include, lib, and output

# INCDIR = inc
# SRCDIR = src
# BLDDIR = build

# CFLAGS += -I$(INCDIR)/
# LDFLAGS += -L$(BLDDIR)/


# # i type 'make clean all' entirely too automatically
# all:
# 	echo if you want this, make release

# # run the all the units
# test:
# 	echo not implemented yet

# # scrub for full build
# clean:
# 	rm -rf release/txb*.h*


# # all the things
# build:  bldda blddl bldkl bldkv bldmd5 bldmisc blpdat bldpmute bldrand bldqu bldrs bldsb bldst bldstr

# bldda: da.h da.c
# 	buildhdr --macro TXBDA --intro LICENSE --pubda.h --privda.c >release/txbda.h

# blddl: dl.h dl.c
# 	buildhdr --macro TXBDL --intro LICENSE --pubdl.h --privdl.c >release/txbdl.h

# bldkl: kl.h kl.c
# 	buildhdr --macro TXBKL --intro LICENSE --pubkl.h --privkl.c >release/txbkl.h

# bldkv: kv.h kv.c
# 	buildhdr --macro TXBKV --intro LICENSE --pubkv.h --privkv.c >release/txbkv.h

# bldmd5: md5.h md5.c
# 	buildhdr --macro TXBMD5 --intro LICENSE --pubmd5.h --privmd5.c >release/txbmd5.h

# bldmisc: misc.h misc.c
# 	buildhdr --macro TXBMISC --intro LICENSE --pubmisc.h --privmisc.c >release/txbmisc.h

# bldpat: pat.h pat.c
# 	buildhdr --macro TXBPAT --intro LICENSE --pubpat.h --privpat.c >release/txbpat.h

# bldpmute: pmute.h pmute.c
# 	buildhdr --macro TXBPMUTE --intro LICENSE --pubpmute.h --privpmute.c >release/txbpmute.h

# bldpq: pq.h pq.c
# 	buildhdr --macro TXBPQ --intro LICENSE --pubpq.h --privpq.c >release/txbpq.h

# bldqu: qu.h qu.c
# 	buildhdr --macro TXBQU --intro LICENSE --pubqu.h --privqu.c >release/txbqu.h

# bldrand: rand.h rand.c
# 	buildhdr --macro TXBRAND --intro LICENSE --pubrand.h --privrand.c >release/txbrand.h

# bldrs: rs.h rs.c
# 	buildhdr --macro TXBRS --intro LICENSE --pubrs.h --privrs.c >release/txbrs.h

# bldsb: sb.h sb.c
# 	buildhdr --macro TXBSB --intro LICENSE --pubsb.h --privsb.c >release/txbsb.h

# bldst: st.h st.c
# 	buildhdr --macro TXBST --intro LICENSE --pubst.h --privst.c >release/txbst.h

# bldstr: str.h str.c
# 	buildhdr --macro TXBSTR --intro LICENSE --pubstr.h --privstr.c >release/txbstr.h


# # install the combined library headers
# install:
# 	cp release/txb*.h ~/.local/include/


# unit:  unitda unitdl unitkl unitkv unitmd5 unitmute unitmisc unitpat unitpq unitqu unitrs unitsb unitst unitstr


# #####################################################
# # these are the libs not as single file headers yet #
# #####################################################

# da.o:  inc/da.h src/da.c
# 	$(CC) $(CFLAGS) -c  src/da.c

# dl.o:  inc/dl.h src/dl.c
# 	$(CC) $(CFLAGS) -c  src/dl.c

# kl.o:  inc/kl.h src/kl.c
# 	$(CC) $(CFLAGS) -c  src/kl.c

# kv.o:  inc/kv.h src/kv.c
# 	$(CC) $(CFLAGS) -c  src/kv.c

# md5.o:  inc/md5.h src/md5.c
# 	$(CC) $(CFLAGS) -c  src/md5.c

# misc.o:  inc/misc.h src/misc.c
# 	$(CC) $(CFLAGS) -c  src/misc.c

# pat.o:  inc/pat.h src/pat.c
# 	$(CC) $(CFLAGS) -c  src/pat.c

# pmute.o:  inc/pmute.h src/pmute.c
# 	$(CC) $(CFLAGS) -c  src/pmute.c

# pq.o:  inc/pq.h src/pq.c
# 	$(CC) $(CFLAGS) -c  src/pq.c

# qu.o:  inc/qu.h src/qu.c inc/str.h
# 	$(CC) $(CFLAGS) -c  src/qu.c

# rand.o:  inc/rand.h src/rand.c
# 	$(CC) $(CFLAGS) -c  src/rand.c

# rs.o:  inc/rs.h src/rs.c inc/str.h
# 	$(CC) $(CFLAGS) -c  src/rs.c

# sb.o:  inc/sb.h src/sb.c inc/str.h
# 	$(CC) $(CFLAGS) -c  src/sb.c

# st.o:  inc/st.h src/st.c inc/dl.h
# 	$(CC) $(CFLAGS) -c  src/st.c

# str.o:  inc/str.h src/str.c
# 	$(CC) $(CFLAGS) -c  src/str.c

# #########################################################
# # these are the test drivers for the various libs,      #
# # they use my own version of minunit.                   #
# #########################################################

# unitda.o:  src/unitda.c
# 	$(CC) $(CFLAGS) -c src/unitda.c

# unitda:  unitda.o  misc.o  da.o  rand.o
# 	$(CC) $(CFLAGS) -o unitda unitda.o  misc.o  da.o  rand.o  $(LDFLAGS)

# unitdl.o: unitdl.c
# 	$(CC) $(CFLAGS) -c unitdl.c

# unitdl: unitdl.o dl.o misc.o str.o  rand.o
# 	$(CC) $(CFLAGS) -o unitdl unitdl.o  dl.o  misc.o  str.o  rand.o  $(LDFLAGS)

# unitkl.o: unitkl.c
# 	$(CC) $(CFLAGS) -c unitkl.c

# unitkl: unitkl.o kl.o misc.o str.o rand.o
# 	$(CC) $(CFLAGS) -o unitkl unitkl.o  kl.o  misc.o  str.o  rand.o  $(LDFLAGS)

# unitkv.o: unitkv.c
# 	$(CC) $(CFLAGS) -c unitkv.c

# unitkv: unitkv.o kv.o rand.o
# 	$(CC) $(CFLAGS) -o unitkv unitkv.o  kv.o  rand.o  $(LDFLAGS)

# unitmd5.o: unitmd5.c
# 	$(CC) $(CFLAGS) -c unitmd5.c

# unitmd5: unitmd5.o md5.o
# 	$(CC) $(CFLAGS) -o unitmd5 unitmd5.o  md5.o  $(LDFLAGS)

# unitmisc.o: unitmisc.c
# 	$(CC) $(CFLAGS) -c unitmisc.c

# unitmisc: unitmisc.o misc.o
# 	$(CC) $(CFLAGS) -o unitmisc unitmisc.o  misc.o  $(LDFLAGS)

# unitmute.o: unitmute.c
# 	$(CC) $(CFLAGS) -c unitmute.c

# unitmute: unitmute.o pmute.o
# 	$(CC) $(CFLAGS) -o unitmute unitmute.o  pmute.o  $(LDFLAGS)

# unitpat.o: unitpat.c
# 	$(CC) $(CFLAGS) -c unitpat.c

# unitpat: unitpat.o misc.o pat.o str.o
# 	$(CC) $(CFLAGS) -o unitpat unitpat.o  misc.o  pat.o  str.o  $(LDFLAGS)

# unitpq.o: unitpq.c
# 	$(CC) $(CFLAGS) -c unitpq.c

# unitpq: unitpq.o pq.o misc.o rand.o str.o
# 	$(CC) $(CFLAGS) -o unitpq unitpq.o  pq.o  misc.o  rand.o  str.o  $(LDFLAGS)

# unitqu.o: unitqu.c
# 	$(CC) $(CFLAGS) -c unitqu.c

# unitqu: unitqu.o qu.o str.o
# 	$(CC) $(CFLAGS) -o unitqu unitqu.o  qu.o  str.o  $(LDFLAGS)

# unitrand.o: unitrand.c
# 	$(CC) $(CFLAGS) -c unitrand.c

# unitrand: unitrand.o rand.o
# 	$(CC) $(CFLAGS) -o unitrand unitrand.o  rand.o  $(LDFLAGS)

# unitrs.o: unitrs.c
# 	$(CC) $(CFLAGS) -c unitrs.c

# unitrs: unitrs.o str.o rs.o
# 	$(CC) $(CFLAGS) -o unitrs unitrs.o  rs.o  str.o  $(LDFLAGS)

# unitsb.o: unitsb.c
# 	$(CC) $(CFLAGS) -c unitsb.c

# unitsb: unitsb.o str.o sb.o
# 	$(CC) $(CFLAGS) -o unitsb unitsb.o  sb.o  str.o  $(LDFLAGS)

# unitst.o: unitst.c
# 	$(CC) $(CFLAGS) -c unitst.c

# unitst: unitst.o st.o dl.o
# 	$(CC) $(CFLAGS) -o unitst unitst.o  st.o  dl.o  $(LDFLAGS)

# unitstr.o: unitstr.c
# 	$(CC) $(CFLAGS) -c unitstr.c

# unitstr: unitstr.o str.o
# 	$(CC) $(CFLAGS) -o unitstr unitstr.o  str.o  $(LDFLAGS)
