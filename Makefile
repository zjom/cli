# Makefile for building tools

# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -O2

# Directories
SRCDIR = src
BINDIR = bin

# Source files (assuming they're in the src directory)
SOURCES = $(wildcard $(SRCDIR)/*.c)

# Object files
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(SRCDIR)/%.o)

# Executables (assuming each .c file corresponds to a tool)
EXECUTABLES = $(SOURCES:$(SRCDIR)/%.c=$(BINDIR)/%)

# List of tool names (without path)
TOOLS = $(notdir $(EXECUTABLES))

# Default target
all: $(EXECUTABLES)

# Rule to create bin directory
$(BINDIR):
	mkdir -p $(BINDIR)

# Rule to build executables
$(BINDIR)/%: $(SRCDIR)/%.o | $(BINDIR)
	$(CC) $(CFLAGS) $< -o $@

# Rule to compile source files
$(SRCDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to build individual tools
$(TOOLS): %: $(BINDIR)/%

# Clean target
clean:
	rm -rf $(BINDIR) $(OBJECTS)

.PHONY: all clean $(TOOLS)
