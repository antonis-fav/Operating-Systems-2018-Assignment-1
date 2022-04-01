SHELL := /bin/bash

# ==================================================
# COMMANDS

CC = gcc
RM = rm -f

# ==================================================
# TARGETS

all: OS

# final link for executable
episode_rename: OS.0
	$(CC) $^ -o $@

# generate objects
%.o: %.c
	$(CC) -c $<

# clean temporary files
clean:
	$(RM) *.o *~

# remove executable
purge: clean
	$(RM)OS
