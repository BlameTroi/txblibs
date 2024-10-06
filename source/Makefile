# Makefile
#
# still trying to get out of source builds figured out
#
# this scripting comes from:
#
# https://riptutorial.com/makefile/example/21376/building-from-different-source-folders-to-different-target-folders
#
# which addresses multiple source directories, everything else i've
# found is too terse for my present level of make understanding or
# single source directory.
#
# projdir is git project root
#
# sourcedir could be a directory holding other sources, and indeed it
# will be
#
# builddir holds all the built stuff
#
# Set project directory one level above of Makefile directory.
#
# $(CURDIR) is a GNU make variable containing the path to the current
# working directory txb so make file goes in source dir? no, build
# dir, with output under it in subdirs
#
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
#
# How to use this Makefile To adapt this Makefile to your project you have to :
#
# Change the TARGET variable to match your target name
# Change the name of the Sources and Build folders in SOURCEDIR and BUILDDIR
# Change the verbosity level of the Makefile in the Makefile itself or in make call
# Change the name of the folders in DIRS to match your sources and build folders
# If required, change the compiler and the flags


# trying to keep Makefile in source, so $curdir/.. is is the git root

PROJDIR := $(realpath $(CURDIR)/..)
SOURCEDIR := $(PROJDIR)/source
BUILDDIR := $(PROJDIR)/build

# Name of the final executable
# txb not for this particular make

TARGET = myApp.exe

# Decide whether the commands will be shown or not

VERBOSE = TRUE

# Create the list of directories

DIRS = src unit
SOURCEDIRS = $(foreach dir, $(DIRS), $(addprefix $(SOURCEDIR)/, $(dir)))
TARGETDIRS = $(foreach dir, $(DIRS), $(addprefix $(BUILDDIR)/, $(dir)))

# Generate the GCC includes parameters by adding -I before each source
# folder

INCLUDES = $(foreach dir, $(SOURCEDIRS), $(addprefix -I, $(dir)))

# Add this list to VPATH, the place make will look for the source
# files

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
CFLAGS = -fsanitize=address -Wall -g3 -O0 -std=c18 --pedantic-errors
LDFLAGS =
# LDFLAGS = -Lm

# OS specific parts

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

# Remove all objects, dependencies and executable files generated
# during the build

clean:
	$(HIDE)$(RMDIR) $(subst /,$(PSEP),$(TARGETDIRS)) $(ERRIGNORE)
	$(HIDE)$(RM) $(TARGET) $(ERRIGNORE)
	@echo Cleaning done !

