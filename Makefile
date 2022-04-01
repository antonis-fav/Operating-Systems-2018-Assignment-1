SHELL := /bin/bash

# ==================================================
# COMMANDS

CC = gcc
RM = rm -f

# ==================================================
# TARGETS

all: Antonis_Favvas

# final link for executable
episode_rename: Antonis_Favvas.0
	$(CC) $^ -o $@

# generate objects
%.o: %.c
	$(CC) -c $<

# clean temporary files
clean:
	$(RM) *.o *~

# remove executable
purge: clean
	$(RM)Antonis_Favvas
