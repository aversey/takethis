# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#     Compilation Options
# Debug mode [yes/no] (allowing to debug the library via gdb):
DEBUG   ?= no
# Specify your favourite C compiler here:
COMPILE ?= gcc
# Are you on Windows?
WINDOWS ?= no


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#     Preparations
# Compile as ANSI C code:
CFLAGS   = -xc -ansi -Wall
# Debug and optimisation (as well as -static for valgrind) are not compatible:
ifeq '$(DEBUG)' 'yes'
CFLAGS  += -g -O0
else
CFLAGS  += -O2
endif
ifeq '$(WINDOWS)' 'yes'
LFLAGS   = -mwindows -lmingw32
endif
# Use SDL:
LFLAGS  += -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_mixer -lm

# Directories definitions:
BUILD    = build
SRCDIR   = src
# Game itself:
GAME     = takethis
# Determing needed object files:
MODULES  = $(foreach x,$(notdir $(wildcard $(SRCDIR)/*.c)),$(x:.c=))
HEADERS  = $(wildcard $(SRCDIR)/*.h)
SRC      = $(foreach i,$(MODULES:=.c),$(SRCDIR)/$(i))
OBJ      = $(foreach i,$(MODULES:=.o),$(BUILD)/$(i))
# Dependency file:
DEPS     = deps.mk


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#     Targets
.PHONY: all clean

all: $(BUILD) $(GAME)

clean:
	rm -f $(GAME) $(OBJ) $(DEPS)


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#     Compilation
ifneq '$(WINDOWS)' 'yes'
-include $(DEPS)
endif

# Packing object files into library:
$(GAME): $(OBJ)
	$(COMPILE) -o $@ $^ $(LFLAGS)

# Compile object files from corresponding source:
$(BUILD)/%.o: $(SRCDIR)/%.c
	$(COMPILE) $(CFLAGS) -c $< -o $@

# Create build directory, if no such:
$(BUILD):
	mkdir $@

# Generate dependency file, adding corresponding build prefixes:
$(DEPS): $(SRC) $(HEADERS)
	$(COMPILE) $(SRC) -MM | sed '/^ /!s#^#$(BUILD)/#' >$@
